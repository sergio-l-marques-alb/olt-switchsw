/** \file egr_queuing.h
 * $Id$
 * 
 * Internal DNX Port APIs 
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef EGR_QUEUING_H_INCLUDED
/** { */
#define EGR_QUEUING_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <bcm/cosq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
/*************
 * DEFINES   *
 *************/
/* { */

/*
 * Number of FAP-data-ports.
 */
#define DNX_NOF_FAP_PORTS             256
#define DNX_MAX_FAP_PORT_ID           (DNX_NOF_FAP_PORTS-1)

#define DNX_COSQ_NOF_TC (8)
#define DNX_COSQ_NOF_DP (4)

#define DNX_COSQ_MIN_COS (4)
#define DNX_COSQ_MAX_COS (8)

#define DNX_COSQ_RESERVED_FLOW_START (0)
#define DNX_COSQ_RESERVED_FLOW_NOF (16)

#define DNX_EGR_QUEUE_DEFAULT_PROFILE_MAP       (0)
#define DNX_EGR_Q_ALMOST_EMPTY_DELAY            (3)
#define DNX_EGQ_COUNTER_INFO_GET(_info, _handle_id, _table, _field, _name, _type, _has_ovf)    \
{                                                                                              \
    sal_snprintf(_info.name, DNX_EGQ_COUNTER_NAME_LEN, "%s", _name);                           \
    _info.type = _type;                                                                        \
    _info.has_ovf = _has_ovf;                                                                  \
    dbal_tables_field_size_get(unit, _table, _field, 0, 0, 0,&_info.counter_size);             \
    dbal_value_field64_request(unit, _handle_id, _field, INST_SINGLE, &(_info.count_val));     \
}
#define DNX_EGQ_SHAPER_MODE_NOF_BITS        (1)

/**
 * PS_MODE values to load into HW to indicate priority mode for a qpair group
 * (port scheduler). See EPS_PS_MODE register (or DBAL_TABLE_
 */
#define DNX_EGQ_PS_MODE_ONE_PRIORITY_VAL    (0)
#define DNX_EGQ_PS_MODE_TWO_PRIORITY_VAL    (1)
#define DNX_EGQ_PS_MODE_FOUR_PRIORITY_VAL   (2)
#define DNX_EGQ_PS_MODE_EIGHT_PRIORITY_VAL  (3)

#define DNX_EGQ_TCG_WEIGHT_MIN              (0)
#define DNX_EGQ_TCG_WEIGHT_MAX              (255)
#define DNX_EGQ_PORT_ID_INVALID             (256)
/**
 * TCG: Number of groups
 */
#define DNX_NOF_TCGS                      (dnx_data_egr_queuing.params.nof_tcg_get(unit))
#define DNX_TCG_MIN                       0
#define DNX_TCG_MAX                       (DNX_NOF_TCGS-1)
#define DNX_EGR_NOF_Q_PAIRS               (dnx_data_egr_queuing.params.nof_q_pairs_get(unit))
#define DNX_EGR_NOF_BASE_Q_PAIRS          (DNX_EGR_NOF_Q_PAIRS)
/**
 * Number of qpairs per one port scheduler.
 */
#define DNX_EGR_NOF_Q_PAIRS_IN_PS         (dnx_data_egr_queuing.params.nof_q_pairs_in_ps_get(unit))
#define DNX_NOF_TCGS_IN_PS                (dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
/**
 * Total number of port schedulers. This is calculated by dividing the total
 * number of 'qpair's by the number of qpairs per one port scheduler.
 * See 'nof_q_pairs_in_ps' and 'nof_q_pairs' in dnx_egr_queuing.xml
 */
#define DNX_EGR_NOF_PS                    (dnx_data_egr_queuing.params.nof_port_schedulers_get(unit))
#define DNX_EGR_NOF_TCG_IDS               (DNX_EGR_NOF_PS * DNX_NOF_TCGS)
#define DNX_EGQ_COUNTER_NAME_LEN          (64)
#define DNX_EGQ_RQP_COUNTER_NUM           (18)
#define DNX_EGQ_PQP_COUNTER_NUM           (8)
#define DNX_EGQ_EPNI_COUNTER_NUM          (5)
#define DNX_EGQ_COUNTER_NUM_SUM           (DNX_EGQ_RQP_COUNTER_NUM + DNX_EGQ_PQP_COUNTER_NUM + 2*DNX_EGQ_EPNI_COUNTER_NUM)

#define DNX_EGQ_NOF_TCG_IN_BITS           (dnx_data_egr_queuing.params.nof_bits_in_nof_tcg_get(unit))

/**
 * Flag to indicate counters that can be configured.
 */
#define DNX_EGQ_PROGRAMMABLE_COUNTER         (0x1)
/**
 * Flag to indicate counters that can not be configured.
 */
#define DNX_EGQ_NON_PROGRAMMABLE_COUNTER     (0x2)
/**
 * Flag to indicate EPNI Queue counters.
 */
#define DNX_EGQ_EPNI_QUEUE_COUNTER           (0x4)
/**
 * Flag to indicate EPNI Interface counters.
 */
#define DNX_EGQ_EPNI_IF_COUNTER              (0x8)
/**
 * Flag to indicate EPNI Mirror counters.
 */
#define DNX_EGQ_EPNI_MIRROR_COUNTER          (0x10)
/**
 * Flag to indicate the configuration of counting bytes.
 */
#define DNX_EGQ_CONFIGURATION_COUNT_BYTES    (0x20)
/**
 * Flag to indicate the configuration of counting packets.
 */
#define DNX_EGQ_CONFIGURATION_COUNT_PACKETS   (0x40)
#define DNX_EGR_OFP_SCH_WFQ_WEIGHT_MAX        255

#define DNX_EGR_SINGLE_MEMBER_TCG_START       (4)
#define DNX_EGR_SINGLE_MEMBER_TCG_END         (7)
/* } */
/*************
 * TYPEDEFS   *
 *************/
/* { */

typedef enum
{
    /*
     * Discrete partition scheme                                                                                                      .
     * Set the thresholds independently. 
     * If the number of allocated resources exceeds the threshold of a given priority,                                                .
     * then all packets with that priority are dropped.
     */
    DNX_EGR_QUEUING_PARTITION_SCHEME_DISCRETE = 0,
    /*
     * Strict partition scheme. Set the thresholds in ascending order. 
     * If the number of allocated resources exceeds a threshold, 
     * Threshold(i) for example,
     * then all packets whose priority is less than i are dropped.  
     */
    DNX_EGR_QUEUING_PARTITION_SCHEME_STRICT = 1,
    /*
     * Total number of partition schemes.
     */
    DNX_NOF_EGR_QUEUING_PARTITION_SCHEMES = 2
} dnx_egr_queuing_partition_scheme_e;

typedef struct
{
    /*
     *  The lower the weight the higher the bandwidth.
     *  Weight of 0 for a class indicates that this class
     *  has SP over the other class.
     *  When both have equal weights it implies simple RR.
     *  Range: 0-255
     */
    uint32 tcg_weight;
    /*
     *  If set, tcg weight is valid and taking part of the
     *  WFQ policy.
     */
    uint8 tcg_weight_valid;

} dnx_egr_tcg_sch_wfq_t;

typedef enum
{
  /**
   *  Port shaper is in data mode.
   */
    DNX_EGR_PORT_SHAPER_DATA_MODE = 0,
  /**
   *  Port shaper is in packet mode.
   */
    DNX_EGR_PORT_SHAPER_PACKET_MODE = 1,
  /**
   *  Total number of Port shaper modes.
   */
    DNX_EGR_NOF_PORT_SHAPER_MODES = 2
} dnx_egr_port_shaper_mode_e;

typedef enum
{
    /*
     *  Port with one priority.
     */
    DNX_EGR_PORT_ONE_PRIORITY = 1,
    /*
     *  Port with two priorities.
     */
    DNX_EGR_PORT_TWO_PRIORITIES = 2,
    /*
     *  Port with two priorities.
     */
    DNX_EGR_PORT_FOUR_PRIORITIES = 4,
    /*
     *  Port with eight priorities.
     */
    DNX_EGR_PORT_EIGHT_PRIORITIES = 8,
    /*
     *  Total number of Port prioritie modes.
     */
    DNX_EGR_NOF_PORT_PRIORITY_MODES = 4
} dnx_egr_port_priority_mode_e;

/**
 * Port id.
 * DNX range: 0-255.
 */
typedef uint32 dnx_fap_port_id_t;

typedef struct
{
    /*
     *  Egress Traffic Class. In general, Range: 0 - 1.
     *  (Equivalent to High (0)/Low (1)). The range can be higher
     *  according to the extended queuing OFP-group
     *  configuration (Possible global maximum: 1, 3, 5 and 7).
     */
    uint32 tc;
    /*
     *  Egress Drop precedence (drop priority). Range: 0 - 3.
     */
    uint32 dp;

} dnx_egr_q_priority_t;

typedef enum
{
    FIRST_DNX_EGR_CAL_TYPE = 0,
    CAL_TYPE_FQP = FIRST_DNX_EGR_CAL_TYPE,
    CAL_TYPE_PQP,
    NUM_DNX_EGR_CAL_TYPE
} dnx_egr_cal_type_e;
/**
 * specifies the selection of Almost empty minimum gap when All Qs under an interface are AE but the interface is not.
 */
typedef enum dnx_egr_ifc_min_gap_e
{
    /*
     * selected for non-channelized interface with 4 or 8 priorities
     */
    DNX_EGR_IFC_MIN_GAP_4_8,
    /*
     * selected for channelized interface
     */
    DNX_EGR_IFC_MIN_GAP_CHANNELIZED,
    /*
     * selected for channelized interface with 1 or 2 priorities
     */
    DNX_EGR_IFC_MIN_GAP_1_2,
    DNX_EGR_IFC_MIN_GAP_MAX
} dnx_egr_ifc_min_gap_e;
/**
 * specifies the Egress multicast replication fifo's types
 */
typedef enum dnx_egr_emr_fifo_e
{
    DNX_EGR_EMR_FIFO_TDM,
    DNX_EGR_EMR_FIFO_UC,
    DNX_EGR_EMR_FIFO_MC_HIGH,
    DNX_EGR_EMR_FIFO_MC_LOW,
    DNX_EGR_EMR_FIFO_MAX
} dnx_egr_emr_fifo_t;
/**
 * specifies the egq counter type
 */
typedef enum dnx_egq_counter_type_e
{
    DNX_EGQ_COUTNER_TYPE_PACKET,
    DNX_EGQ_COUTNER_TYPE_BYTE
} dnx_egq_counter_type_t;
/**
 * specifies the egq counter filter configuration
 */
typedef enum dnx_egq_counter_filter_e
{
    DNX_EGQ_COUTNER_FILTER_BY_QP,
    DNX_EGQ_COUTNER_FILTER_BY_OTM,
    DNX_EGQ_COUTNER_FILTER_BY_IF,
    DNX_EGQ_COUTNER_FILTER_BY_MIRROR,
    DNX_EGQ_COUTNER_FILTER_BY_NONE
} dnx_egq_counter_filter_t;
/**
 * data structure for egq counter information
 */
typedef struct
{
    char name[DNX_EGQ_COUNTER_NAME_LEN];
    dnx_egq_counter_type_t type;
    uint64 count_val;
    int has_ovf;
    int count_ovf;
    int counter_size;
} dnx_egq_counter_info_t;
typedef struct
{
    /*
     * Service pool eligibility. Indicates if it can use shared resources.
     * Range: 0 - 1.
     */
    uint32 pool_eligibility;
    /*
     * Service Pool index.
     * Range: 0 - 1.
     */
    uint32 pool_id;
    /*
     * Traffic Class group for service pool thresholds.
     * Range: 0 - 7.
     */
    uint32 tc_group;
} dnx_egr_queuing_mc_cos_map_t;

typedef enum
{
    FIRST_DNX_EGR_Q_PRIO_MAPPING_TYPE = 0,
    /**
     *  Mapping from Traffic Class and Drop Precedence to Egress
     *  Queue Priority:Unicast packets to scheduled queues.
     */
    DNX_EGR_UCAST_TO_SCHED = FIRST_DNX_EGR_Q_PRIO_MAPPING_TYPE,
    /**
     *  Multicast packets to unscheduled queues (unscheduled
     *  multicast).
     */
    DNX_EGR_MCAST_TO_UNSCHED,
    /**
     *  Must be the last value. Indicates the number of [Traffic
     *  Class, Drop Precedence] to Egress Queue Priority Mapping
     *  modes.
     */
    NUM_DNX_EGR_Q_PRIO_MAPPING_TYPE
} dnx_egr_q_prio_mapping_type_e;

typedef struct
{
    /*
     * Schedule (UC) words consumed per priority 
     */
    dnx_egr_q_priority_t queue_mapping[NUM_DNX_EGR_Q_PRIO_MAPPING_TYPE][DNX_COSQ_NOF_TC][DNX_COSQ_NOF_DP];
} dnx_cosq_egress_queue_mapping_info_t;
/* 
 * TCG Traffic class groups. Range: 0-7
 */
typedef uint32 dnx_tcg_ndx_t;
typedef enum
{
    /*
     *  Egress priority for unscheduled traffic - low
     */
    DNX_EGR_Q_PRIO_LOW = 0,
    /*
     *  Egress priority for unscheduled traffic - high
     */
    DNX_EGR_Q_PRIO_HIGH = 1,
    /*
     *  Number of egress priorities for unscheduled traffic - petra B.
     */
    DNX_EGR_NOF_Q_PRIO_PB = 2,
    /*
     *  Egress priority in DNX 0 - 7
     */
    DNX_EGR_Q_PRIO_0 = 0,
    DNX_EGR_Q_PRIO_1 = 1,
    DNX_EGR_Q_PRIO_2 = 2,
    DNX_EGR_Q_PRIO_3 = 3,
    DNX_EGR_Q_PRIO_4 = 4,
    DNX_EGR_Q_PRIO_5 = 5,
    DNX_EGR_Q_PRIO_6 = 6,
    DNX_EGR_Q_PRIO_7 = 7,
    /*
     *  Number of egress priorities for unscheduled traffic.
     */
    DNX_EGR_NOF_Q_PRIO,
    /*
     * All priorities: for thresholds set for High + Low priorities
     */
    DNX_EGR_Q_PRIO_ALL = 0xFFFF
} dnx_egr_q_prio_e;
typedef struct
{
    /*
     *  TCG (Traffic class groups) that the q-pair is mapped to.
     *   Range: 0 - 7                                                                                                  .
     */
    dnx_tcg_ndx_t tcg_ndx[DNX_EGR_NOF_Q_PRIO];
} dnx_egr_queuing_tcg_info_t;

typedef struct
{
    /*
     *  Unscheduled, or Unscheduled weight. The lower the
     *  weight the higher the bandwidth. Weight of 0 for a class
     *  indicates that this class has SP over the other class.
     *  When both have equal weights it implies simple RR.
     *  Range: 0-255
     */
    uint32 unsched_weight;
    /*
     *  Scheduled, or Scheduled weight. Format -same as
     *  unscheduled. Range: 0 - 255.
     */
    uint32 sched_weight;

} dnx_egr_ofp_sch_wfq_t;

typedef enum
{
    FIRST_DNX_EGR_OFP_INTERFACE_PRIO = 0,
    /*
     *  Outgoing FAP Strict Priority, for egress scheduling -
     *  high. Note: this priority is only valid for ports mapped
     *  to channelized NIF port! Otherwise - ignored.
     */
    DNX_EGR_OFP_INTERFACE_PRIO_HIGH = FIRST_DNX_EGR_OFP_INTERFACE_PRIO,
    /*
     *  OFP priority - medium
     *  Not valid for JER2_ARAD.
     */
    DNX_EGR_OFP_INTERFACE_PRIO_MID,
    /*
     *  OFP priority - low
     */
    DNX_EGR_OFP_INTERFACE_PRIO_LOW,
    /*
     *  OFP priority - lowest.
     *  Applicable for Soc_petra-B only.
     *  For Soc_petra-B: should be used for eight-priorities FC configuration.
     *  Only one OFP mapped to a given NIF is allowed to be set to this priority.
     */
    DNX_EGR_OFP_INTERFACE_PRIO_PFC_LOWEST,
    /*
     *  Must be the last value. Indicates the number of OFP
     *  scheduling priorities.
     */
    DNX_EGR_OFP_CHNIF_NOF_PRIORITIES
} dnx_egr_ofp_interface_prio_e;

typedef struct
{
    uint32 mc_or_mc_low_queue_weight;
    uint32 uc_or_uc_low_queue_weight;
} dnx_egq_dwm_tbl_data_t;

typedef struct
{
    uint32 lb_key_max;
    uint32 lb_key_min;
    uint32 second_range_lb_key_max;
    uint32 second_range_lb_key_min;
    uint32 is_stacking_port;
    uint32 peer_tm_domain_id;
    uint32 port_type;
    uint32 cnm_intrcpt_fc_vec_llfc;
    uint32 cnm_intrcpt_fc_vec_pfc;
    uint32 cnm_intrcpt_fc_en;
    uint32 cnm_intrcpt_drop_en;
    uint32 ad_count_out_port_flag;
    uint32 cgm_port_profile;    /* Threshold type */
    uint32 cgm_interface;
    uint32 base_q_pair_num;
    uint32 cos_map_profile;
    uint32 pmf_data;
    uint32 disable_filtering;
} dnx_egq_ppct_tbl_data_t;

typedef struct
{
    /*
     * 3b 
     */
    uint32 map_profile;
    /*
     * 1b 
     */
    uint32 is_egr_mc;
    /*
     * 3b 
     */
    uint32 tc;
    /*
     * 2b 
     */
    uint32 dp;

} dnx_egq_tc_dp_map_tbl_entry_t;
typedef struct
{
    uint32 tc;
    uint32 dp;
} dnx_egq_tc_dp_map_tbl_data_t;

typedef int dnx_port_t;

typedef struct
{
  /**
   *  Strict Priority, if mapped to channelized
   *  Interface. Otherwise - ignored by the HW.
   */
    dnx_egr_ofp_interface_prio_e nif_priority;
  /**
   *  WFQ weights. The WFQ is among low priority traffic
   *  (scheduled/unscheduled), or among all scheduled and
   *  unscheduled traffic - according to the OFP egress
   *  scheduling mode. Not valid for DNX.
   */
    dnx_egr_ofp_sch_wfq_t ofp_wfq;
  /**
   *  WFQ weights. The WFQ is among high priority traffic
   *  (scheduled/unscheduled). Relevant only if schedule
   *  mode is HP_OVER_LP_FAIR.
   *  Not valid for Soc_petra-A, DNX.
   */
    dnx_egr_ofp_sch_wfq_t ofp_wfq_high;
  /**
   *  WFQ weights. The WFQ is among priority traffic
   *  (scheduled/unscheduled). Each index corresponds
   *  to q-pair.
   *  Valid for DNX only.
   */
    dnx_egr_ofp_sch_wfq_t ofp_wfq_q_pair[DNX_EGR_NOF_Q_PRIO];
} dnx_egr_ofp_sch_info_t;

/* } */
/****************
 * PROTOTYPES   *
 ****************/
/* { */

int dnx_egr_queuing_default_tc_dp_map_set(
    int unit,
    bcm_port_t port);
int dnx_egr_port2egress_offset(
    int unit,
    int core,
    uint32 tm_port,
    uint32 *egr_if);
/**
 * \brief - sets bandwidth for egress recycle channel (mirror/recycle)
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  core_id
 * \param [in] is_mirror - channel is mirror recycle port
 * \param [in] speed - channel rate in kbps
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_recycle_bandwidth_set(
    int unit,
    int core,
    int is_mirror,
    uint32 speed);
/**
 * \brief - gets bandwidth for egress recycle channel (mirror/recycle)
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  core_id
 * \param [in] is_mirror - channel is mirror recycle port
 * \param [out] speed - channel rate in kbps
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_recycle_bandwidth_get(
    int unit,
    int core,
    int is_mirror,
    uint32 *speed);
/**
 * \brief - sets burst for egress recycle channel (mirror/recycle)
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  core_id
 * \param [in] is_mirror - channel is mirror recycle port
 * \param [in] burst - channel burst in bytes
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_recycle_burst_set(
    int unit,
    int core,
    int is_mirror,
    uint32 burst);
/**
 * \brief - gets burst for egress recycle channel (mirror/recycle)
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  core_id
 * \param [in] is_mirror - channel is mirror recycle port
 * \param [out] burst - channel burst in bytes
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_recycle_burst_get(
    int unit,
    int core,
    int is_mirror,
    uint32 *burst);
/**
 * \brief - gets weight for egress recycle channel (mirror/recycle)
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  core_id
 * \param [in] is_mirror - channel is mirror recycle port
 * \param [in] weight - channel weight
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_recycle_weight_set(
    int unit,
    int core,
    int is_mirror,
    uint32 weight);
/**
 * \brief - sets weight for egress recycle channel (mirror/recycle)
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  core_id
 * \param [in] is_mirror - channel is mirror recycle port
 * \param [out] weight - channel weight
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_recycle_weight_get(
    int unit,
    int core,
    int is_mirror,
    uint32 *weight);
/**
 * \brief - gets port to egress recycle interface mapping
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [out] rcy_if_idx - recycle interface index
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_port_recycle_interface_map_get(
    int unit,
    bcm_port_t port,
    int *rcy_if_idx);
/**
 * \brief - setting compensation for logical port
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [in] comp -  compensation value
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_port_compensation_set(
    int unit,
    bcm_port_t port,
    int comp);
/**
 * \brief - setting compensation for logical port
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [in] comp -  compensation value
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_port_compensation_get(
    int unit,
    bcm_port_t port,
    int *comp);

/**
 * \brief - egr_queuing implementation nif credit init
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_nif_credit_default_set(
    int unit,
    bcm_port_t port);

/**
* \brief - sets tcg cir sp. configuration per core
*
* \param [in] unit -  Unit-ID
* \param [in] gport -  passed to retrieve core_id
* \param [in] enable - enable
*
* \return
*   shr_error_e
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_egr_queuing_tcg_cir_sp_en_set(
    int unit,
    bcm_gport_t gport,
    uint32 enable);
/**
* \brief - gets tcg cir sp. configuration per core
*
* \param [in] unit -  Unit-ID
* \param [in] gport -  passed to retrieve core_id
* \param [out] enable - enable
*
* \return
*   shr_error_e
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_egr_queuing_tcg_cir_sp_en_get(
    int unit,
    bcm_gport_t gport,
    uint32 *enable);
/**
 * \brief - sets the IRDY & txq_max_bytes threshold per speed. The value retrieved from dnx_data
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_if_thr_set(
    int unit,
    bcm_port_t port);
/**
 * \brief - Configures PQP & FQP calendars
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  core_id
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_nif_calendar_set(
    int unit,
    bcm_core_t core);
/**
 * \brief - egr_queuing implementation for bcm_port_add API
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_port_add_handle(
    int unit);
/**
 * \brief - egr_queuing implementation for bcm_port_remove API
 * 
 *
 * \param [in] unit -  Unit-ID
 *   
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_port_remove_handle(
    int unit);
/**
 * \brief - Function to set SP before WFQ mode (per OTM port) 
 *
 * \param [in] unit -  Unit-ID 
 * \param [in] port -  Port 
 * \param [in] enable - Enable or disable the SP before WFQ mode
 *        (per OTM port)
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_prio_over_cast_set(
    int unit,
    bcm_port_t port,
    int enable);
/**
 * \brief - Function to get SP before WFQ mode (per OTM port) 
 *
 * \param [in] unit -  Unit-ID 
 * \param [in] port -  Port 
 * \param [out] enable - Indicates if the SP before WFQ mode 
 *        (per OTM port) enabled or disabled
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_prio_over_cast_get(
    int unit,
    bcm_port_t port,
    int *enable);
/**
 * \brief - enable/disable port qpairs. flush option is available for disable mode only.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [in] enable -  enable
 * \param [in] flush -  whether to do flush or not
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_port_enable_and_flush_set(
    int unit,
    bcm_port_t port,
    uint32 enable,
    uint32 flush);
/**
 * \brief - get the port qpairs enable status.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [out] enable -  enable status
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_port_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable);
/**
 * \brief - get egq rqp counter information .
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 * \param [out] counter_info -  store counter information
 * \param [out] nof_counter -  return number of counters
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_rqp_counter_info_get(
    int unit,
    int core,
    dnx_egq_counter_info_t * counter_info,
    int *nof_counter);
/**
 * \brief - configure egq pqp counter parameters.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 * \param [in] count_by -  indicate count by qp, hr or egq if
 * \param [in] count_place -  could be qp, hr_id or egq_if, which is decided by count_by.
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_pqp_counter_configuration_set(
    int unit,
    int core,
    int count_by,
    int count_place);
/**
 * \brief - clear egq pqp counter .
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_pqp_counter_configuration_reset(
    int unit,
    int core);
/**
 * \brief - get egq pqp counter information .
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 * \param [out] counter_info -  store counter information
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_pqp_counter_info_get(
    int unit,
    int core,
    dnx_egq_counter_info_t * counter_info);
/**
 * \brief - configure egq rqp counter parameters.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 * \param [in] flag -  Flag
 * \param [in] count_by -  indicate count by qp, hr or egq if
 * \param [in] count_place -  could be qp, hr_id or egq_if, which is decided by count_by.
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_epni_counter_configuration_set(
    int unit,
    int core,
    int flag,
    int count_by,
    int count_place);
/**
 * \brief - clear egq epni counter .
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_epni_counter_configuration_reset(
    int unit,
    int core);
/**
 * \brief - get egq epni counter information .
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core-ID
 * \param [in] flag -  flag
 * \param [out] counter_info -  store counter information
 * \param [out] nof_counter -  return number of counters
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_epni_counter_info_get(
    int unit,
    int core,
    int flag,
    dnx_egq_counter_info_t * counter_info,
    int *nof_counter);
/**
 * \brief - disable egress queue port and flush it to make sure that the queue is empty
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * Called from port_remove sequence
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_port_disable(
    int unit);
/**
 * \brief - egr_queuing module init
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_init(
    int unit);
/**
 * \brief - egr_queuing module deinit
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_deinit(
    int unit);
/**
 * \brief - allocate egress interface
 *
 * \param [in] unit -  Unit ID
 * \param [in] port -  logical port
 * \param [in] master_logical_port -  master port in case channelized or DNX_ALGO_PORT_INVALID otherwise
 * \param [in] core - core ID
 * \param [in] interface_type -  interface type of port
 * \param [out] egr_if -  egress interface ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_queuing_interface_alloc(
    int unit,
    bcm_port_t port,
    int master_logical_port,
    bcm_core_t core,
    bcm_port_if_t interface_type,
    int *egr_if);
void dnx_egr_queuing_tcg_info_clear(
    dnx_egr_queuing_tcg_info_t * info);

void dnx_egr_ofp_sch_info_clear(
    dnx_egr_ofp_sch_info_t * info);

/*
 * This header is TEMPORARY.
 * Remove it when procedure becomes 'static'
 */
void dnx_egr_queuing_mc_cos_map_clear(
    dnx_egr_queuing_mc_cos_map_t * info);
/*********************************************************************
* NAME:
*     dnx_egr_ofp_scheduling_set
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*     Set per-port egress scheduling information.
* INPUT:
*  int                 unit -
*     Identifier of the device to access.
*  int                 core -
*     Identifier of the core to access.
*  uint32              tm_port -
*     TM Port index to configure.
*  uint32              cosq -
*     
*  dnx_egr_ofp_sch_info_t    *info -
*     Per-port egress scheduling info
* REMARKS:
*   Some of the scheduling info may be irrelevant -
*   i.e. channelized interface priority is only relevant if the port
*   is mapped to a channelized NIF.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int dnx_egr_ofp_scheduling_set(
    int unit,
    int core,
    uint32 tm_port,
    uint32 cosq,
    dnx_egr_ofp_sch_info_t * info);
/*********************************************************************
* NAME:
 *   dnx_egr_queuing_ofp_tcg_get 
 * TYPE:
 * PROC
 *    Associate the queue-pair (Port,Priority) to traffic class
 *    groups (TCG) attributes.
 * Input
 *   int                       unit -
 *     Identifier of the device to access.
 *  int                        core -
 *     Identifier of the core to access.
 *  uint32                     tm_port -
 *     TM Port index to inquire.
 *   uint32                    tcg_info -
 *     TCG attributes to be filled/read from
 * REMARKS:
 *   The last four TCG are single-member groups. Scheduling within
 *   a TCG is done in a strict priority manner according to the
 *   priority level. (e.g. If P1,P2,P7 within the same TCG1.
 *   Then SP select is descending priority P7,P2,P1).
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
int dnx_egr_queuing_ofp_tcg_get(
    int unit,
    int core,
    uint32 tm_port,
    dnx_egr_queuing_tcg_info_t * tcg_info);

/**
 * \brief
 *    Set a single mapping element which indicates, for specified port,  to which TCG,
 *    specified priority is mapped.
 * \param [in] unit -
 *    Int. Identifier of HW platform.
 * \param [in] core -
 *    Int. Identifier of the core to access.
 * \param [in] tm_port -
 *    uint32. TM Port index to configure.
 * \param [in] priority -
 *    dnx_egr_q_prio_e. Priority (one of 8) to use in mapping TCG. Essentially,
 *    indicated TCG is mapped to 'priority' which is one of 8 entries
 *    assigned to 'tm_port'.
 * \param [in] tcg_to_map
 *    dnx_tcg_ndx_t. The TCG to map to indicated priority and tm_port.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * dnx_egr_queuing_ofp_tcg_set
 *   * dnx_egr_queuing_ofp_tcg_get
 */
shr_error_e dnx_egr_queuing_ofp_tcg_single_set(
    int unit,
    int core,
    uint32 tm_port,
    dnx_egr_q_prio_e priority,
    dnx_tcg_ndx_t tcg_to_map);
/**
 * \brief
 *    Get a single mapping element which indicates, for specified port,  to which TCG,
 *    specified priority is mapped.
 * \param [in] unit -
 *    Int. Identifier of HW platform.
 * \param [in] core -
 *    Int. Identifier of the core to access.
 * \param [in] tm_port -
 *    uint32. TM Port index to configure.
 * \param [in] priority -
 *    uint32. Priority (one of 8) to use in mapping TCG. Essentially,
 *    indicated TCG is mapped to 'priority' which is one of 8 entries
 *    assigned to 'tm_port'.
 * \param [out] tcg_to_map_p
 *    Pointer to dnx_tcg_ndx_t.
 *    This procedure loads pointed memory by the TCG to map to indicated priority
 *    and tm_port.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * dnx_egr_queuing_ofp_tcg_set
 *   * dnx_egr_queuing_ofp_tcg_get
 */
shr_error_e dnx_egr_queuing_ofp_tcg_single_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 priority,
    dnx_tcg_ndx_t * tcg_to_map_p);
/**
 * \brief
 *    See dnx_egr_ofp_scheduling_set
 * \param [in] unit -
 *    Int. Identifier of HW platform.
 * \param [in] core -
 *    Int. Identifier of the core to access.
 * \param [in] tm_port -
 *    uint32. TM Port index to configure.
 * \param [in] cosq -
 *    uint32
 * \param [in] info -
 *    Pointer to dnx_egr_ofp_sch_info_t. Per-port egress scheduling info
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *   Some of the scheduling info may be irrelevant -
 *   i.e. channelized interface priority is only relevant if the port
 *   is mapped to a channelized NIF.
*********************************************************************/
int dnx_egr_ofp_scheduling_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 cosq,
    dnx_egr_ofp_sch_info_t * info);
/*********************************************************************
* NAME:
 *   dnx_egr_q_prio_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets egress queue priority per traffic class and drop
 *   precedence.
 * INPUT:
 *   int                     unit -
 *     Identifier of the device to access.
 *   int                        core -
 *     Identifier of the core to access.
 *   dnx_egr_q_prio_mapping_type_e map_type_ndx -
 *     mapping type, defining what traffic type
 *     (unicast/multicast) is mapped to what egress queue type
 *     (scheduled/unscheduled).
 *   uint32                     tc_ndx -
 *     Traffic Class, as embedded in the packet header. Range:
 *     0 - 7.
 *   uint32                     dp_ndx -
 *     Drop Precedence, as embedded in the packet header.
 *     Range: 0 - 3.
 *   uint32                     map_profile_ndx -
 *     Mapping profile index. Range: 0 - 3.
 *   dnx_egr_q_priority_t             *priority -
 *     Egress Queuing priority (Drop precedence and Traffic
 *     class).
 * REMARKS:
 *   Each OFP is mapped to an egress queue profile by the
 *   dnx_egr_q_profile_map_set API.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
int dnx_egr_q_prio_set(
    int unit,
    int core,
    dnx_egr_q_prio_mapping_type_e map_type_ndx,
    uint32 tc_ndx,
    uint32 dp_ndx,
    uint32 map_profile_ndx,
    dnx_egr_q_priority_t * priority);
/*********************************************************************
*     Gets the configuration set by the
 *     "dnx_egr_q_prio_set" API.
 *     Refer to "dnx_egr_q_prio_set" API for details.
*********************************************************************/
int dnx_egr_q_prio_get(
    int unit,
    int core,
    dnx_egr_q_prio_mapping_type_e map_type_ndx,
    uint32 tc_ndx,
    uint32 dp_ndx,
    uint32 map_profile_ndx,
    dnx_egr_q_priority_t * priority);
/*********************************************************************
* NAME:
 *   jer2_arad_egr_q_profile_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Function description
 * INPUT:
 *   int                      unit -
 *     Identifier of the device to access.
 *   int                      core_id -
 *     Identifier of the core to access.
 *   dnx_fap_port_id_t              ofp_ndx -
 *     Outgoing Fap Port index. Range: 0 - 79.
 *   uint32                      map_profile_id -
 *     Egress queue priority profile index. Range: 0 - 3.
 * REMARKS:
 *   The profile configuration is set by the
 *   jer2_arad_egr_q_prio_set API.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
int dnx_egr_q_profile_map_set(
    int unit,
    int core_id,
    uint32 tm_port,
    uint32 map_profile_id);
/*********************************************************************
*     Gets the configuration set by the
 *     "dnx_egr_q_profile_map_set" API.
 *     Refer to "dnx_egr_q_profile_map_set" API for
 *     details.
*********************************************************************/
int dnx_egr_q_profile_map_get(
    int unit,
    int core_id,
    uint32 tm_port,
    uint32 *map_profile_id);
/*********************************************************************
* NAME:
 *   dnx_egr_dsp_pp_priorities_mode_set
 * \param [in] unit -
 *    Int. Identifier of HW platform.
 * \param [in] core -
 *    Int. Identifier of the core to access.
 * \param [in] tm_port -
 *    uint32. TM Port index to configure.
 * \param [in] priority_mode -
 *    Enumration dnx_egr_port_priority_mode_e. Priority mode to
 *    assign to this port (Essentially number of priorities: One of 1,2,4,8)
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
*********************************************************************/
int dnx_egr_dsp_pp_priorities_mode_set(
    int unit,
    int core_id,
    uint32 tm_port,
    dnx_egr_port_priority_mode_e priority_mode);
/*********************************************************************
 * is_high_priority indicates if port is high or low  priority
*********************************************************************/
shr_error_e dnx_egr_queuing_is_high_priority_queue_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 cosq,
    int *is_high_priority);
void dnx_egr_tcg_sch_wfq_clear(
    dnx_egr_tcg_sch_wfq_t * info);
/*********************************************************************
* NAME:
*     dnx_sch_port_tcg_weight_set/get 
* TYPE:
*   PROC
* DATE:
*  
* FUNCTION:
*     Sets, for a specified TCG within a certain OFP
*     its excess rate. Excess traffic is scheduled between other TCGs
*     according to a weighted fair queueing or strict priority policy.
*     Set invalid, in case TCG not take part of this policy.
* INPUT:
*  int                        unit -
*     Identifier of device to access.
*  dnx_fap_port_id_t          ofp_ndx -
*     Port id, 0 - 255. 
*  dnx_tcg_ndx_t              tcg_ndx -
*     TCG index. 0-7.
*  dnx_egr_tcg_sch_wfq_t      *tcg_weight -
*     TCG weight information.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   This function must only be called for eight priorities port.
*********************************************************************/
int dnx_egr_queuing_tcg_weight_set(
    int unit,
    int core,
    uint32 tm_port,
    dnx_tcg_ndx_t tcg_ndx,
    dnx_egr_tcg_sch_wfq_t * tcg_weight);
int dnx_egr_queuing_tcg_weight_get(
    int unit,
    int core,
    uint32 tm_port,
    dnx_tcg_ndx_t tcg_ndx,
    dnx_egr_tcg_sch_wfq_t * tcg_weight);
void dnx_egr_q_priority_clear(
    dnx_egr_q_priority_t * info);
/*********************************************************************
*     Enable/disable the egress shaping.
*********************************************************************/
int dnx_dbg_egress_shaping_enable_set(
    int unit,
    uint8 enable);
int dnx_dbg_egress_shaping_enable_get(
    int unit,
    uint8 *enable);
/* } */

/** } */
#endif /* EGR_QUEUING_H_INCLUDED */
