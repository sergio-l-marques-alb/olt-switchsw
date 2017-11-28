/** \file utilex_seq.h
 * 
 * Provide the a utlity that helps to run a sequence backward and forward.
 * Adittional tools provided:
 * * Sequence Logging
 * * Time tracking
 * * Time testing
 * * Tests for memory leaks
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef UTILEX_SEQ_H_INCLUDED
#define UTILEX_SEQ_H_INCLUDED
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/time.h>

/*
 * }
 */
/*
 * MACROs
 * {
 */
/*
 * Per step flag
 * {
 */
/** 
 * \brief 
 * Skip the step
 * Useful while dynmically disabling a step
 * In this case, the flag will be provided by a callback (flags callback)
 */
#define UTILEX_SEQ_STEP_F_SKIP                         (0x1)
/** 
 * \brief 
 * Skip Step due to WB
 */
#define UTILEX_SEQ_STEP_F_WB_SKIP                      (0x2)
/** 
 * \brief 
 * Set logging severity for this step to verbose
 */
#define UTILEX_SEQ_STEP_F_VERBOSE                      (0x4)
/** 
 * \brief 
 *  The step is dependent on a SOC property that can mask the
 *  step and is enabled by default.
 */
#define UTILEX_SEQ_STEP_F_SOC_PROP_EN                  (0x8)
/** 
 * \brief 
 *  The step is dependent on a SOC property that can mask the
 *  step and is disabled by default.
 */
#define UTILEX_SEQ_STEP_F_SOC_PROP_DIS                 (0x10)
/** 
 * \brief 
 *  This flag enables the step to be ran as standalone (without
 *  any of the other steps in the list).
 */
#define UTILEX_SEQ_STEP_F_STANDALONE_EN                (0x20)
/**
 * \brief 
 * step required for access only initalization (allow lite inializtion with access only)
 */
#define UTILEX_SEQ_STEP_F_REQUIRED_FOR_ACCESS_ONLY      (0x40)
/**
 * \brief 
 * step required for access only (including per port access) initalization (allow lite with access only)
 */
#define UTILEX_SEQ_STEP_F_REQUIRED_FOR_PORT_ACCESS_ONLY (0x80)

/*
 * }
 */

/**
 * \brief - mark invalid step id
 */
#define UTILEX_SEQ_STEP_INVALID         (0)
/**
 * \brief - id of last step in each sub list
 */
#define UTILEX_SEQ_STEP_LAST            (-1)

/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */
/**
 * \brief utilex_intenal data that will be saved per step.
 *        should not be set by the user.
 */
typedef struct
{
  /** 
   * step starting time stamp
   */
    sal_usecs_t time_stamp_start;

  /** 
   * step ending time stamp
   */
    sal_usecs_t time_stamp_end;

  /** 
   * step starting memory alocation 
   */
    int total_memory_alocation_start;

  /** 
   * step ending memory alocation 
   */
    int total_memory_alocation_end;
  /** 
   * Memory alocated for SW state
   */
    uint32 swstate_allocated_recources;

   /**
    * Flags - 
    * Sum of all flags - static, dynamic and internal
    */
    uint32 total_flags;

} utilex_seq_step_internal_t;

/** 
 * \brief ptr to step cb function returning shr_error_e, to be 
 * used in each forward/backward step. the step_list_info and the 
 * flags are used for internal purposes, and are managed by the 
 * mechanism. 
 */
typedef shr_error_e(
    *utilex_seq_cb_f) (
    int unit);
/**
 * \brief ptr to step flag cb function returning 
 * shr_error_e, to be used in each init step to decode flags 
 * according to a given step logic ( for example Step X should 
 * be skipped if SOC property Y is set to value Z etc). to do 
 * so, one would need to return in dynamic flags the 
 * DNX_INIT_STEP_F_SKIP flag. 
 */
typedef shr_error_e(
    *utilex_seq_flag_cb_f) (
    int unit,
    int *dynamic_flags);

/**
 * \brief step/sub-step structure.
 *  Provide callbacks for sequnce forward and backward.
 *  Provide additional parametres for advanced utilities.
 *  Note that the structure must init by the function 'utilex_seq_step_t_init()'
 * 
 */
typedef struct utilex_seq_step_s utilex_seq_step_t;
struct utilex_seq_step_s
{
  /** 
   * Step ID, used to uniquely identify a step. 
   */
    int step_id;

  /**
   * Step name
   */
    char *step_name;

  /** 
   * Forward function CB - used when running the seqeunce forward. 
   */
    utilex_seq_cb_f forward;

  /** 
   * Backward function CB - used when running the seqeunce backward. 
   */
    utilex_seq_cb_f backward;

  /** 
   * Flag function CB that will run prior to the forward/backward function of 
   * this step to determine which flags are needed to the step 
   * according to the CB logic - could be looking for certain SOC 
   * properties for example.
   * See UTILEX_SEQ_STEP_F_*
   */
    utilex_seq_flag_cb_f dyn_flags;

  /** 
   * Step Flags, internal flags used by the system's logic
   * See UTILEX_SEQ_STEP_F_*
   */
    int static_flags;

  /** 
   * SOC property suffix - together with the control_prefix of the 
   *  list, used for creating the SOC property that will
   * enable/disable the step if the appropriate flag is set.
   */
    char *soc_prop_suffix;

  /** 
   * Time threshold - the maximum time the step should take in 
   * microseconds. If set to 0 the default value will be used.
   */
    sal_time_t time_thresh;

  /** 
   * Pointer to a sub list of steps to be used if current step 
   * represents a sub list of steps. 
   */
    utilex_seq_step_t *step_sub_array;

    /**
     * Internal fields - used to store additional statistics and more about this step.
     * Should not be set by utilex_seq user!
     */
    utilex_seq_step_internal_t internal_info;
};

/**
 * \brief Global info about the sequence list.
 *
 */

typedef struct
{
  /**
   * pointer to the main step list.
   */
    utilex_seq_step_t *step_list;

  /**
   * Used when want to run partial sequence, in that case the first step and last step should be provided explicitly
   */
    int first_step;

  /**
   * Used when want to run partial sequence, in that case the first step and last step should be provided explicitly
   */
    int last_step;
   /**
    * Control Prefix - prefix for Soc property to enable/disable 
    * steps in the list. 
    */
    char *control_prefix;
   /**
    * Log Prefix - prefix for each log that will be done by the utility.
    */
    char *log_prefix;

   /**
    * BSL Flag to be used for logging.
    * See BSL_LS_* macros
    */
    uint32 bsl_flags;

    /**
     * Logging Severity 
     */
    bsl_severity_t log_severity;

    /**
     * Enable time_logging;
     */
    int time_log_en;

    /**
     * Enable time testing - will be failed if a step time cross the provided threshold.
     */
    int time_test_en;

    /**
     * For time testing - holds the threshold for the overall time it
     * took to run the sequence in microseconds 
     */
    sal_time_t time_thresh;

    /**
     * Enable memory logging;
     */
    int mem_log_en;

    /**
     * Enable memory testing - will be failed if a memory leakage identified.
     */
    int mem_test_en;

    /**
     * Enable memory logging for SW state;
     */
    int swstate_log_en;

    /**
     * Enable memory test for SW state will be failed if SW state 
     * takes up more memory than expected
     */
    int swstate_test_en;

    /**
     * Warmboot indication - indicates warmboot process is on.
     * This attribute should not be modified and relevant 
     */
    int warmboot;
    /**
     * Access only booting
     * Will run just steps marked with UTILEX_SEQ_STEP_F_REQUIRED_FOR_ACCESS_ONLY and UTILEX_SEQ_STEP_F_REQUIRED_FOR_ACCESS_ONLY
     */
    int access_only;
    /**
     * Store last step passed - used for error recovery
     */
    int last_passed_step;
} utilex_seq_t;

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */
/**
 * \brief - initialize main structure
 *          must be called when using utilex_seq
 * \param [in] unit - unit #.
 * \param [out] seq - the structure to be initlized.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_seq_t_init(
    int unit,
    utilex_seq_t * seq);

/**
 * \brief - initialize step sequrnce structure
 *          must be called when defining a step
 * \param [in] unit - unit #.
 * \param [out] step - the structure to be initlized.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_seq_step_t_init(
    int unit,
    utilex_seq_step_t * step);

/**
 * \brief - function to run a specific step from a list as 
 *        standalone by its name
 *  
 * \param [in] unit - unit #
 * \param [in] step_list - ptr to relevant step list  
 * \param [in] step_name - name of step to run
 * \param [in] forward - indicates weather to run forward 
 *        (STEP_INIT) of backward (STEP_DEINIT) function
 * \param [out] step_found - indicates weather the step was 
 *        found or not
 *  
 * \return 
 *  See shr_error_e 
 * \see
 *   * None
 */
shr_error_e utilex_seq_run_step_by_name(
    int unit,
    utilex_seq_step_t * step_list,
    char *step_name,
    int forward,
    int *step_found);

/**
 * \brief - function to run a specific step from a list as 
 *        standalone by its id
 *  
 * \param [in] unit - unit #
 * \param [in] step_list - ptr to relevant step list  
 * \param [in] step_id - ID of step to run
 * \param [in] forward - indicates weather to run forward 
 *        (STEP_INIT) of backward (STEP_DEINIT) function
 * \param [out] step_found - indicates weather the step was 
 *        found or not
 *  
 * \return 
 *  See shr_error_e 
 * \see
 *   * None
 */
shr_error_e utilex_seq_run_step_by_id(
    int unit,
    utilex_seq_step_t * step_list,
    int step_id,
    int forward,
    int *step_found);

/**
 * \brief - function to run a specific step from a list as 
 *        standalone
 * 
 * \param [in] unit - Unit ID
 * \param [in] current_step - The step to run
 * \param [in] forward - Forward or backward
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_seq_run_step(
    int unit,
    utilex_seq_step_t * current_step,
    int forward);

/**
 * \brief - run sequence backward or forward
 *          According to the configuration provided in seq.
 * \param [in] unit - unit #.
 * \param [in] seq - sequence configuration.
 * \param [in] forward - whether to run list backward or forward.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_seq_run(
    int unit,
    utilex_seq_t * seq,
    int forward);

/*
 * }
 */

#endif /* UTILEX_SEQ_H_INCLUDED */
