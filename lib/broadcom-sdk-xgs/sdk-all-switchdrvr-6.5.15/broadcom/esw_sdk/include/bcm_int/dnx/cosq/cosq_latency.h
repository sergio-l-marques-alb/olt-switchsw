/*
 * ! \file bcm_int/dnx/cosq/cosq_latency.h
 * Reserved.$ 
 */

#ifndef _DNX_COSQ_LATENCY_H_INCLUDED_
/*
 * { 
 */
#define _DNX_COSQ_LATENCY_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/cosq.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

/*
 * Defines
 * {
 */
#define DNX_COSQ_LATENCY_INGRESS_RESOURCE "COSQ_LATENCY_INGRESS"
#define DNX_COSQ_LATENCY_END_TO_END_RESOURCE "COSQ_LATENCY_END_TO_END"
#define DNX_COSQ_LATENCY_QUARTET_QUEUES_TEMPLATE_NAME "cosq latency quartet queues profiles"

/*
 * }
 */

/**
* \brief
*   init cosq latency sub-module.
* \param [in] unit -unit id
* \return
*    shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_cosq_latency_init(
    int unit);

/**
* \brief
*   de-init cosq latency sub-module.
* \param [in] unit -unit id
* \return
*    shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_cosq_latency_deinit(
    int unit);

/**
* \brief
*  function get gport and a single property of the latency profile and call the relevant function to set it.
* \param [in] unit -unit id
* \param [in] port - latency gport
* \param [in] cosq - offset/index  of the object.relevant just for type bin
* \param [in] type - property type of the profile
* \param [in] arg  - the value of the property
* \return
*    shr_error_e
* \remark
*   None
* \see
*   NONE
*/
shr_error_e dnx_cosq_latency_profile_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);

/**
* \brief
*  function get gport and get a single property value of the latency profile
* \param [in] unit -unit id
* \param [in] port - latency gport
* \param [in] cosq - offset/index  of the object.relevant just for type bin
* \param [in] type - property type of the profile
* \param [out] arg  - the value of the property
* \return
*    shr_error_e
* \remark
*   None
* \see
*   NONE
*/
shr_error_e dnx_cosq_latency_profile_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);

/**
 * \brief
 *      callback print function for template manager. print the queue quartet latency profile
 * \param [in] unit -unit id
 * \param [in] data -const pointer to profile data
 * \param [in] print_cb_data -
 *      Pointer of the print callback data.
 * \return
 *  void
 * \remark
 *   NONE
 * \see
 *   NONE
 */
void dnx_cosq_latency_queue_template_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
* \brief
*  handle the mapping from queue (base of 4) to latency profile.
* \param [in] unit -unit id
* \param [in] flags - NONE
* \param [in] qid - the queue id to map.
* \param [in] lat_profile_id - the latency profile that the queue will be mapped to
* \return
*    shr_error_e
* \remark
*   The queue quartet profile contain two properties: {TC_MAP, latency-profile}.
*   This routine map queue to queue quartet profile and queue quartet profile to latency-profile.
*   the TC_MAP is not being handled in this function.
*   Since we are using template manager, we need to give all properties of the profile. 
*   therefore, we read the TC_MAP from WH and build the tm info based on the user lat_profile_id and tc_map.
* \see
*   NONE
*/
shr_error_e dnx_cosq_latency_ingress_queue_map_set(
    int unit,
    int flags,
    int qid,
    int lat_profile_id);

/**
* \brief
*  get the mapped latency profile for a given base quartet queue
* \param [in] unit -unit id
* \param [in] flags - NONE
* \param [in] qid - the queue id to map.
* \param [out] lat_profile_id - the latency profile that the queue is mapped to
* \return
*    shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_cosq_latency_ingress_queue_map_get(
    int unit,
    int flags,
    int qid,
    int *lat_profile_id);

/**
* \brief
*  determine if end to end latency is per port or per flow
* \param [in] unit -unit id
* \param [in] enable - TRUE/FALSE
* \return
*    shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_cosq_latency_end_to_end_per_port_set(
    int unit,
    int enable);

/**
* \brief
*  get if end to end latency is per port or per flow
* \param [in] unit -unit id
* \param [in] enable - TRUE/FALSE
* \return
*    shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_cosq_latency_end_to_end_per_port_get(
    int unit,
    int *enable);

/**
* \brief
*  map a gport (destination port) to end to end latency profile
* \param [in] unit -unit id
* \param [in] flags - NONE
* \param [in] gport_to_map - destination port
* \param [in] profile_id - latency profile_id
* \return
*    shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_cosq_latency_end_to_end_port_map_set(
    int unit,
    uint32 flags,
    bcm_gport_t gport_to_map,
    int profile_id);

/**
* \brief
*  get the end to end latency profile id that the gport (destination port) is map to.
* \param [in] unit -unit id
* \param [in] flags - NONE
* \param [in] gport_to_map - destination port
* \param [out] profile_id - latency profile_id
* \return
*    shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_cosq_latency_end_to_end_port_map_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport_to_map,
    int *profile_id);

/** } */
#endif /*_DNX_COSQ_LATENCY_H_INCLUDED_ */
