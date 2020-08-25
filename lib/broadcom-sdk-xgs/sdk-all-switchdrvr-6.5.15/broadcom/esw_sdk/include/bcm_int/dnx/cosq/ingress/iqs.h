/** \file bcm_int/dnx/cosq/ingress/iqs.h
 * $Id$
 *  
 * Module IQS - Ingress Queue Scheduling 
 *  
 * Configuration of Ingress TM queue scheduling, inclues: 
 *  * Credit request profiles
 *  * Credit worth
 *  * FSM reorder mechanism
 *  
 *  Not including:
 *  * FMQ Credit generation(legacy implementation)
 * 
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _DNX_IQS_INCLUDED__
#define _DNX_IQS_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <shared/shrextend/shrextend_error.h>
#include <bcm/cosq.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

/*
 * Defines:
 * {
 */

/*
 * } 
 */
/*
 * Typedefs:
 * {
 */

/**
 * \brief - SRAM or S2D deq parameters threshold types
 */
typedef enum
{
    /*
     * Below max deq cmd
     */
    DNX_IQS_DEQ_PARAM_TYPE_MAX_DEQ_CMD_BELOW,

    /*
     * Above max deq cmd
     */
    DNX_IQS_DEQ_PARAM_TYPE_MAX_DEQ_CMD_ABOVE,

    /*
     * Above 150% od max deq cmd
     */
    DNX_IQS_DEQ_PARAM_TYPE_MAX_DEQ_CMD_ABOVE_EXTREME,

    DNX_IQS_DEQ_PARAM_TYPE_NOF
} dnx_iqs_deq_param_type_e;

/** 
 * \brief -  
 * FSM (Flow Status Message) modes
 */
typedef enum
{
    /**
     * Add sequential number to FSM cell in order to handle reordering (recommended)
     */
    dnx_iqs_fsm_mode_sequential = 0,
    /**
     * Wait before sending new FSM in order to prevent reordering.
     */
    dnx_iqs_fsm_mode_delayed = 1,

    /**
     * Must be last
     */
    dnx_iqs_fsm_mode_nof
} dnx_iqs_fsm_mode_e;

/**
 * \brief Ingress congestion voq state info.
 */
typedef struct
{
    /**
     * ID of current queue
     */
    dbal_enum_value_field_queue_credit_request_state_e credit_request_state;
    /**
     * Total size in Bytes of current VOQ
     */
    uint32 credit_balance;
} dnx_iqs_voq_state_info_t;

/*
 * } 
 */
/*
 * Module Init
 * {
 */

/**
 * \brief
 *   Initialize dnx iqs module.
 * 
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_init(
    int unit);

/**
 * \brief
 *   Deinitialize dnx iqs module.
 * 
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_deinit(
    int unit);
/*
 * } 
 */

 /*
  * Credit request profiles
  * (Just the functions used by other modules)
  * {
  */
/**
 * \brief - 
 * Verify that the used profile_id already set once
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] profile_id - credit request profile id
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_request_profile_valid_verify(
    int unit,
    int profile_id);
/**
 * \brief - 
 * Map ingress queue to credit request profile.
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] queue_id - ingress queue_id of base_queue
 * \param [in] queue_offset - ingress queue offset  inside a bundle 
 * \param [in] profile_id - credit request profile id
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_request_profile_map_set(
    int unit,
    int queue_id,
    int queue_offset,
    int profile_id);

/**
 * \brief - 
 * Get the mapping of ingress queue to credit request profile.
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] queue_id - ingress queue_id of base queue
 * \param [in] queue_offset - ingress queue offset  inside a bundle
 * \param [out] profile_id - credit request profile id
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_request_profile_map_get(
    int unit,
    int queue_id,
    int queue_offset,
    int *profile_id);

  /*
   * }
   */
/*
 * Watchdog Period
 * (Just the functions used by fabric APIs)
 * {
 */
/**
 * \brief - 
 * Set min queue id for watchdog mechanism to scan.
 * Note that this function set sw data base only.
 * In order to configure the hw accordingly - call to dnx_iqs_credit_watchdog_hw_set()
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] min_queue_id - min queue id
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_watchdog_queue_min_sw_set(
    int unit,
    int min_queue_id);

/**
 * \brief - 
 * Get min queue id for watchdog mechanism to scan.
 * 
 * \param [in] unit -  Unit-ID 
 * \param [out] min_queue_id - min queue id
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_watchdog_queue_min_sw_get(
    int unit,
    int *min_queue_id);

/**
 * \brief - 
 * Set max queue id for watchdog mechanism to scan.
 * Note that this function set sw data base only.
 * In order to configure the hw accordingly - call to dnx_iqs_credit_watchdog_hw_set()
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] min_queue_id - min queue id
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_watchdog_queue_max_sw_set(
    int unit,
    int min_queue_id);

/**
 * \brief - 
 * Get max queue id for watchdog mechanism to scan.
 * 
 * \param [in] unit -  Unit-ID 
 * \param [out] min_queue_id - min queue id
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_watchdog_queue_max_sw_get(
    int unit,
    int *min_queue_id);
/**
 * \brief - 
 * Enable / Disable credit watchdog mechanism.
 * Note that this function set sw data base only.
 * In order to configure the hw accordingly - call to dnx_iqs_credit_watchdog_hw_set()
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] enable - 1 to enable, 0 to disable
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_watchdog_enable_sw_set(
    int unit,
    int enable);

/**
 * \brief - 
 * Enable / Disable credit watchdog mechanism.
 * 
 * \param [in] unit -  Unit-ID 
 * \param [out] enable - 1 to enable, 0 to disable
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_watchdog_enable_sw_get(
    int unit,
    int *enable);

/**
 * \brief - 
 * Set Watchdog mechanism HW according to watchdog mechanism sw state.
 * 
 * \param [in] unit -  Unit-ID 
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_watchdog_hw_set(
    int unit);

/**
 * \brief - 
 * Verify credit watchdog configuration
 * 
 * \param [in] unit -  Unit-ID 
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_watchdog_verify(
    int unit);

/*
 * } 
 */
/*
 * Credit Worth
 * { 
 */
/**
 * \brief - 
 * There are two groups of credit worth.
 * Get credit worth  of each group.
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] group_id -  group 0 or group 1 
 * \param [out] credit_worth_bytes -  credit worth in bytes 
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_credit_worth_group_get(
    int unit,
    int group_id,
    uint32 *credit_worth_bytes);
/*
 * } 
 */
/*
 * FSM mode
 * Used to prevent FSM reordering
 * { 
 */
/**
 * \brief - 
 * Set FSM mode per destination FAP-ID (sequential or delayed)
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] fap_id -  destination fap id
 * \param [in] fsm_mode -  sequential or delayed, see enum
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_fsm_mode_set(
    int unit,
    uint32 fap_id,
    dnx_iqs_fsm_mode_e fsm_mode);
/**
 * \brief - 
 * Get FSM mode per destination FAP-ID (sequential or delayed)
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] fap_id -  destination fap id
 * \param [in] fsm_mode -  sequential or delayed, see enum
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_fsm_mode_get(
    int unit,
    uint32 fap_id,
    dnx_iqs_fsm_mode_e * fsm_mode);
/*
 * }
 */
/*
 * Debug
 * { 
 */
/**
 * \brief - 
 * Enable / Disable auto-credit over a range of queues
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] queue_min -  min queue range. for disable  set to 0.
 * \param [in] queue_max -  max queue range. for disable  set to 0.
 * \param [in] enable -  enable / disable auto-credit.
 *             
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_debug_autocredit_enable_set(
    int unit,
    uint32 queue_min,
    uint32 queue_max,
    uint32 enable);

/**
 * \brief -
 * Enable / Disable auto-credit over a range of queues
 *
 * \param [in] unit -  Unit-ID
 * \param [out] queue_min -  min queue range.
 * \param [out] queue_max -  max queue range.
 * \param [out] enable -  enable / disable auto-credit.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_iqs_debug_autocredit_enable_get(
    int unit,
    uint32 *queue_min,
    uint32 *queue_max,
    uint32 *enable);

/**
 * \brief -
 * Convert rate to predefined bandwidth levels
 *
 * \param [in] unit -  Unit-ID
 * \param [in] rate -  port rate - Gbps.
 * \param [out] bw_level -  level according to rate.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_rate_to_bw_level_get(
    int unit,
    uint32 rate,
    int *bw_level);
/**
 * \brief -
 * Convert rate to required credit resolution
 *
 * \param [in] unit -  Unit-ID
 * \param [in] bw_level - level #
 * \param [out] credit_resolution -  required credit resolution
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_iqs_bw_level_to_credit_resolution_get(
    int unit,
    int bw_level,
    int *credit_resolution);

/**
 * \brief - Function to get queue state signals and credit request info
 *
 * \param [in] unit - unit index
 * \param [in] voq_gport - voq_gport
 * \param [in] cos - vos offset in the bundle
 * \param [out] voq_state_info - voq credit state info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 * \see
 *   * None
 */
shr_error_e dnx_iqs_voq_state_info_get(
    int unit,
    bcm_gport_t voq_gport,
    int cos,
    dnx_iqs_voq_state_info_t * voq_state_info);
/*
 * }
 */

#endif/*_DNX_IQS_INCLUDED__*/
