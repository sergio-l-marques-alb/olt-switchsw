/**
 * \file bcm_int/dnx/cosq/cosq.h
 * $Id$
 *  
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef COSQ_H_INCLUDED
#define COSQ_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <shared/shrextend/shrextend_error.h>
#include <bcm/cosq.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_sch_config_access.h>
/*
 * Defines:
 * {
 */
/*
 * Port with the following number of priorities supports TCG
 */
#define DNX_TCG_NOF_PRIORITIES_SUPPORT           (8)
#define DNX_COSQ_TCG_NOF_PRIORITIES_SUPPORT      (DNX_TCG_NOF_PRIORITIES_SUPPORT)

#define DNX_COSQ_EGR_OFP_SCH_SP_LOW              (BCM_COSQ_SP1)
#define DNX_COSQ_EGR_OFP_SCH_SP_HIGH             (BCM_COSQ_SP0)

#define DNX_COSQ_EGR_OFP_SCH_WFQ_WEIGHT_MAX      (255)

#define DNX_MULT_FABRIC_NOF_BE_CLASSES           3
/*
 * COSQ max num for PB
 */
#define DNX_COSQ_PB_EGR_OFP_COSQ_NUM_MAX(unit)   (BCM_COS_MAX)
/*
 * credit egress dicsount limitations
 */
#define DNX_COSQ_EGRESS_CR_DISCOUNT_MIN_VAL      (-32)
#define DNX_COSQ_EGRESS_CR_DISCOUNT_MAX_VAL      (32)
/*
 * Bandwidth limits
 */
#define DNX_COSQ_PB_EGR_OFP_RATE_MAX_BURST_LIMIT (0xFFFF)
/*
 * See dnx_egr_q_prio_mapping_type_e
 */
#define  DNX_DEVICE_COSQ_EGR_NOF_Q_PRIO_MAPPING_TYPES   (NUM_DNX_EGR_Q_PRIO_MAPPING_TYPE)

#define  DNX_DEVICE_COSQ_ING_NOF_TC                     (8)
#define  DNX_DEVICE_COSQ_ING_NOF_DP                     (4)

#define  DNX_DEVICE_COSQ_EGR_NOF_DP                     (4)
#define  DNX_DEVICE_COSQ_EGR_NOF_TC                     (8)

#define DNX_DEVICE_COSQ_VSQ_NOF_RSRC_POOLS(unit)        (2)

#define DNX_DEVICE_COSQ_NBR_QUEUES_IN_QUARTET           (dnx_data_ipq.queues.min_bundle_size_get(unit))
#define DNX_COSQ_QUEUE_QUAD_GET(qid)                    (qid / DNX_DEVICE_COSQ_NBR_QUEUES_IN_QUARTET)
/*
 * } 
 */
/*
 * Typedefs:
 * {
 */
typedef struct bcm_dnx_cosq_egress_thresh_key_info_s
{

    /*
     * Soc_dnx B
     */

    /*
     * Schedule (UC) words consumed per priority - Soc_dnx-B
     */
    uint32 sch_drop_words_consumed[DNX_EGR_NOF_Q_PRIO];
    /*
     * Unschedule (MC) words consumed per {priority,dp} - Soc_dnx-B
     */
    uint32 unsch_drop_words_consumed[DNX_EGR_NOF_Q_PRIO][DNX_DEVICE_COSQ_EGR_NOF_DP];
    /*
     * Schedule (UC) pkts consumed per priority - Soc_dnx-B 
     */
    uint32 sch_drop_pkts_consumed[DNX_EGR_NOF_Q_PRIO];
    /*
     * Unschedule (MC) pkts consumed per {priority,dp} - Soc_dnx-B
     */
    uint32 unsch_drop_pkts_consumed[DNX_EGR_NOF_Q_PRIO][DNX_DEVICE_COSQ_EGR_NOF_DP];
    /*
     * Schedule (UC) flow control words consumed per priority - Soc_dnx-B
     */
    uint32 sch_fc_words_consumed[DNX_EGR_NOF_Q_PRIO];

    /*
     * JER2_ARAD 
     */

    /*
     * Schedule (UC) packet descriptors - JER2_ARAD 
     */
    uint32 sch_drop_packet_descriptors;
    /*
     * Schedule (UC) packet descriptors per tc - JER2_ARAD 
     */
    uint32 sch_drop_packet_descriptors_tc[DNX_DEVICE_COSQ_EGR_NOF_TC];
    /*
     * Unschedule (MC) packet descriptors per - JER2_ARAD 
     */
    uint32 unsch_drop_packet_descriptors;
    /*
     * Unschedule (MC) packet descriptors per {tc,dp} - JER2_ARAD
     */
    uint32 unsch_drop_packet_descriptors_tc_dp[DNX_DEVICE_COSQ_EGR_NOF_TC][DNX_DEVICE_COSQ_EGR_NOF_DP];
    /*
     * Schedule (UC) data buffers - JER2_ARAD
     */
    uint32 sch_drop_data_buffers;
    /*
     * Schedule (UC) data buffers per tc - JER2_ARAD
     */
    uint32 sch_drop_data_buffers_tc[DNX_DEVICE_COSQ_EGR_NOF_TC];
    /*
     * Unschedule (MC) data buffers - JER2_ARAD
     */
    uint32 unsch_drop_data_buffers;
    /*
     * Unschedule (MC) data buffers per {tc,dp} - JER2_ARAD
     */
    uint32 unsch_drop_data_buffers_tc_dp[DNX_DEVICE_COSQ_EGR_NOF_TC][DNX_DEVICE_COSQ_EGR_NOF_DP];
    /*
     * Schedule (UC) flow control packet descriptors - JER2_ARAD
     */
    uint32 sch_fc_packet_descriptors;
    /*
     * Schedule (UC) flow control packet descriptors per tc - JER2_ARAD
     */
    uint32 sch_fc_packet_descriptors_tc[DNX_DEVICE_COSQ_EGR_NOF_TC];
    /*
     * Schedule (UC) flow control data buffers - JER2_ARAD
     */
    uint32 sch_fc_data_buffers;
    /*
     * Schedule (UC) flow control data buffers per tc - JER2_ARAD
     */
    uint32 sch_fc_data_buffers_tc[DNX_DEVICE_COSQ_EGR_NOF_TC];
    /*
     * Unschedule (MC) data buffers available per tc - JER2_ARAD
     */
    uint32 unsch_drop_packet_descriptors_available_tc[DNX_DEVICE_COSQ_EGR_NOF_TC];
    uint32 port_fc_packet_descriptors;
    uint32 port_fc_data_buffers;

    /*
     * JER2_JERICHO 
     */
    /*
     * Min unschedule (MC) packet descriptors - JER2_JERICHO 
     */
    uint32 unsch_drop_packet_descriptors_min;
    /*
     * Alpha for dynamic threshold unschedule (MC) packet descriptors - JER2_JERICHO 
     */
    int32 unsch_drop_packet_descriptors_alpha;
    /*
     * Min and Alpha for Port level - FC
     */
    uint32 port_fc_packet_descriptors_min;
    int32 port_fc_packet_descriptors_alpha;
    uint32 port_fc_data_buffers_min;
    int32 port_fc_data_buffers_alpha;

    /*
     * Min unschedule (MC) packet descriptors per {tc,dp} - JER2_JERICHO
     */
    uint32 unsch_drop_packet_descriptors_min_tc_dp[DNX_DEVICE_COSQ_EGR_NOF_TC][DNX_DEVICE_COSQ_EGR_NOF_DP];
    /*
     * Alpha for dynamic threshold unschedule (MC) packet descriptors per {tc,dp} - JER2_JERICHO
     */
    int32 unsch_drop_packet_descriptors_alpha_tc_dp[DNX_DEVICE_COSQ_EGR_NOF_TC][DNX_DEVICE_COSQ_EGR_NOF_DP];

    uint32 sch_fc_packet_descriptors_min_tc[DNX_DEVICE_COSQ_EGR_NOF_TC];
    int32 sch_fc_packet_descriptors_alpha_tc[DNX_DEVICE_COSQ_EGR_NOF_TC];
    uint32 sch_fc_data_buffers_min_tc[DNX_DEVICE_COSQ_EGR_NOF_TC];
    int32 sch_fc_data_buffers_alpha_tc[DNX_DEVICE_COSQ_EGR_NOF_TC];

} bcm_dnx_cosq_egress_thresh_key_info_t;

typedef struct
{
    /*
     *  The maximum generation credit rate for the best
     *  effort/guaranteed traffic queues. For a credit size of
     *  512 Bytes, and a core frequency of 250 MHZ, the range:
     *  Minimum - 123 Kbps, Maximum - 75 Gbps
     */
    uint32 rate;
    /*
     *  The maximum number of credits the generator can hold
     *  when credits are not needed. Range: 0 - 63.
     */
    uint32 max_burst;
} dnx_mult_fabric_shaper_info_t;

typedef struct
{
    /*
     *  HR Id mapped to the Multicast class in the scheduler.
     */
    dnx_fap_port_id_t mcast_class_hr_id;
    /*
     *  If True, then the scheduler receives credits for the
     *  port mapped to this Multicast class. Otherwise, the
     *  credits are directly given to the queue.
     */
    int multicast_class_valid;
} dnx_mult_fabric_class_sch_info_t;

typedef struct
{
    /*
     *  Scheduling port configuration for a scheduling scheme
     *  (per MC class). Must be set only if the credits come to
     *  the FMQs via the scheduler.
     */
    dnx_mult_fabric_class_sch_info_t be_sch;
    /*
     *  The proportion in which credits are generated. Range: 0
     *  - 15. (Max to min).
     */
    uint32 weight;
} dnx_mult_fabric_be_class_info_t;

typedef struct
{
    /*
     *  Shaper configuration of the credit generation.
     */
    dnx_mult_fabric_shaper_info_t be_shaper;
    /*
     *  If True, then the credit will be distributed according
     *  to the weights per Multicast Class. Otherwise, they are
     *  distributed in a strict priority in the following order:
     *  MC2 > MC1 > MC0.
     */
    uint8 wfq_enable;
    /*
     *  Best effort scheduling port configuration (per MC class)
     */
    dnx_mult_fabric_be_class_info_t be_sch[DNX_MULT_FABRIC_NOF_BE_CLASSES];
} dnx_mult_fabric_be_info_t;

typedef struct
{
    /*
     *  Shaper configuration of the credit generation.
     */
    dnx_mult_fabric_shaper_info_t gr_shaper;
    /*
     *  Scheduling port configuration for a scheduling scheme.
     *  Must be set only if the credits come to the queues via
     *  the scheduler.
     */
    dnx_mult_fabric_class_sch_info_t gr_sch;
} dnx_mult_fabric_gr_info_t;

typedef struct
{
    /*
     *  Credit configuration for the guaranteed traffic (FMQ 3).
     */
    dnx_mult_fabric_gr_info_t guaranteed;
    /*
     *  Credit configuration for the best effort traffic (FMQ 0
     *  - 2).
     */
    dnx_mult_fabric_be_info_t best_effort;
    /*
     *  The maximum rate in which credits are generated. Units:
     *  Kbps. For a credit size of 512 Bytes, and a core
     *  frequency of 250 MHZ, the range: Minimum - 123 Kbps,
     *  Maximum - 75 Gbps
     */
    uint32 max_rate;
    /*
     *  The maximum number of credits the generator can hold
     *  when credits are not needed. Range: 0 - 63.
     *  Valid for DNX only.
     */
    uint32 max_burst;
    /*
     *  If True, then the credits come to the FMQs via
     *  scheduling schemes handled by the scheduler. Otherwise,
     *  the credits go directly to the FMQs and no scheme must
     *  be set in the scheduler. Must be set to True to enable
     *  the Enhanced configuration.
     */
    uint8 credits_via_sch;
} dnx_mult_fabric_info_t;

typedef enum
{
    DNX_COSQ_THRESHOLD_INDEX_INVALID = -1,
    DNX_COSQ_THRESHOLD_INDEX_DP,
    DNX_COSQ_THRESHOLD_INDEX_TC,
    DNX_COSQ_THRESHOLD_INDEX_POOL_DP,
    DNX_COSQ_THRESHOLD_INDEX_CAST_PRIO_FDWACT,
    DNX_COSQ_THRESHOLD_INDEX_PRIO,
    DNX_COSQ_THRESHOLD_INDEX_POOL_PRIO
} dnx_cosq_threshold_index_type_t;

typedef struct
{
    int dp;
    int tc;
    int pool_id;
    bcm_cast_t cast;
    bcm_cosq_ingress_forward_priority_t priority;
    bcm_cosq_forward_decision_type_t fwd_action;
} dnx_cosq_threshold_index_info_t;

typedef struct
{
    /*
     *  Passing this threshold from above means activation.
     */
    uint32 set;
    /*
     *  Passing this threshold from below means deactivation.
     */
    uint32 clear;
} dnx_cosq_hyst_threshold_t;

typedef struct
{
    /**
     * Maximum FADT threshold
     */
    uint32 max_threshold;
    /**
     * Minimum FADT threshold
     */
    uint32 min_threshold;
    /**
     * If AdjustFactor3 is set,  
     *    Dynamic-Max-Th = Free-Resource << AdjustFactor2:0 
     * Otherwise, 
     *    Dynamic-Max-Th = Free-Resource >> AdjustFactor2:0
     */
    int alpha;
} dnx_cosq_fadt_threshold_t;

typedef struct
{
    /**
     *  Passing this threshold from above means activation.
     *  The threshold is dynamic (FADT).
     */
    dnx_cosq_fadt_threshold_t set;
    /**
     *  Passing this threshold from below means deactivation.
     *  The threshold is an offset below the FADT set threshold.
     */
    uint32 clear_offset;
} dnx_cosq_fadt_hyst_threshold_t;
 /*
  * } 
  */
/*
 * Functions
 * {
 */
int dnx_cosq_bit_count_get(
    uint32 value);

/**
 * \brief - internal function for setting static (non FADT) threshold,
 *          this function supports all available thresh_info->index types, unlike the BCM API
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - additional flags. currently not in use
 *   \param [in] thresh_info - complementary information describing the threshold to be set
 *   \param [in] threshold - threshold value
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_cosq_gport_static_threshold_internal_set(
    int unit,
    uint32 flags,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold);

/**
 * \brief - internal function getting static (non FADT) threshold
 *          this function supports all available thresh_info->index types, unlike the BCM API
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - additional flags. currently not in use
 *   \param [in] thresh_info - complementary information describing the threshold to be get
 *   \param [in] threshold - obtained threshold value
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_cosq_gport_static_threshold_internal_get(
    int unit,
    uint32 flags,
    bcm_cosq_static_threshold_info_t * thresh_info,
    bcm_cosq_static_threshold_t * threshold);

/*
 * }
 */

/**
 * \brief - get the queue id and core from gport type queue
 * \param [in] unit - unit
 * \param [in] gport_queue -  gport to retrieve the information for
 * \param [in] cosq - offset of the queue group
 * \param [in] force_core_all - indicate if to verify that the core is BCM_CORE_ALL
 * \param [out] core - core the queue belong
 * \param [out] queue_id - queue id
 * \return
 *    shr_error_e
 * \remark
 *   None
 * \see
 *   NONE
 */
shr_error_e dnx_cosq_gport_to_voq_get(
    int unit,
    bcm_gport_t gport_queue,
    bcm_cos_queue_t cosq,
    int force_core_all,
    bcm_core_t * core,
    int *queue_id);
/**
 * \brief -
 * verify gport and cosq for rate class threshold APIs
 *
 * \param [in] unit - unit index
 * \param [in] gport - gport to verify. BCM_GPORT_IS_PROFILE for rate class profile. if is_vsq_allowed is set BCM_COSQ_GPORT_IS_VSQ is also accepted.
 * \param [in] cosq - cosq to verify. expected 0 or -1
 * \param [in] is_vsq_allowed - indication whether to allow VSQ Gport as well.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
int dnx_cosq_rate_class_gport_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int is_vsq_allowed);

/*
 * Exposed legacy functions during rewrite process
 * These procedures are still waiting to be ported to DNX
 * {
 */
int _bcm_dnx_cosq_vsq_gl_fc_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 flags,
    bcm_cosq_pfc_config_t * pfc_threshold);

int _bcm_dnx_cosq_vsq_gl_fc_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 flags,
    bcm_cosq_pfc_config_t * pfc_threshold);
/* 
 * Purpose : Set weight, sp between FMQ class
 */
int _bcm_dnx_cosq_gport_fmq_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight);
int _bcm_dnx_cosq_control_fmq_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);

int _bcm_dnx_cosq_control_fmq_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);
int _bcm_dnx_cosq_gport_fmq_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags);
int _bcm_dnx_cosq_gport_fmq_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags);

/* 
 * Purpose : Retrieve weight, sp between FMQ class
 */
int _bcm_dnx_cosq_gport_fmq_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight);
/*
 * }
 */

#endif /* COSQ_H_INCLUDED */
