/** \file bcm_int/dnx/algo/multicast/algo_multicast.h
 *
 * Reserved.$
 */

#ifndef _DNX_ALGO_MULTICSAT_INCLUDED_
/*
 * {
 */
#define _DNX_ALGO_MULTICSAT_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/types/sw_state_res_mngr.h>
#include <soc/dnx/swstate/auto_generated/access/algo_failover_access.h>

/*
 * MACROs
 * {
 */

/*
 * }
 * MACROs
 */
/*
 * enum for multicast MCDB allocation method
 */
typedef enum
{
    /*
     * At this method there is only one region and ingress and egress can use the entire range of MCDB
     */
    DNX_ALGO_MULTICAST_ALLOCATION_BASIC
} dnx_algo_multicast_mcdb_allocation_method_e;
/*
 * }
 */

/**
 * \brief Wrapper function for allocation manager which creates tagged allocation bitmap for multicast mcdb.
 *           The tag will represent ingress or egress replication,
 *           Refer to dnx_algo_res_simple_bitmap_create() for details about the input parameters.
 */
shr_error_e dnx_algo_multicast_bitmap_create(
    int unit,
    uint32 module_id,
    sw_state_resource_tag_bitmap_t * res_tag_bitmap,
    int core_id,
    int sub_resource_index,
    sw_state_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags);

#endif /*_DNX_ALGO_MULTICSAT_INCLUDED_*/
