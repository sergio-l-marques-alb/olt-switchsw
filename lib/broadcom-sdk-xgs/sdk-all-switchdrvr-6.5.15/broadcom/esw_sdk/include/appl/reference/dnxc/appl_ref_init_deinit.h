/** \file appl_ref_init_deinit.h
 *
 * init and deinit functions to be used by the INIT_DNX command.
 *
 * need to make minimal amount of steps before we are able to use call bcm_init
 *
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef FILE_APPL_REF_DNXC_INIT_DEINIT_H_INCLUDED
/* { */
#define FILE_APPL_REF_DNXC_INIT_DEINIT_H_INCLUDED

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNXC family only!"
#endif

#include <appl/diag/diag.h>
#include <shared/utilex/utilex_seq.h>
#include <appl/reference/dnxc/appl_ref_init_utils.h>

#ifndef AGGRESSIVE_ALLOC_DEBUG_TESTING
  #define AGGRESSIVE_ALLOC_DEBUG_TESTING 0
#endif

/*
* MACROs:
* {
*/
/**
 * brief - Default flags for applications steps. 
 * By default each application should have the following behaviour: 
 * 1. Can be run as standalone (useful for running steps from diag command shell) 
 * 2. Is enabled during init. 
 */
#define DEF_PROP_EN (UTILEX_SEQ_STEP_F_STANDALONE_EN | UTILEX_SEQ_STEP_F_SOC_PROP_EN)

#define WB_SKIP     (UTILEX_SEQ_STEP_F_WB_SKIP)

#define STANDALONE  (UTILEX_SEQ_STEP_F_STANDALONE_EN)

#define ACCESS      (UTILEX_SEQ_STEP_F_REQUIRED_FOR_ACCESS_ONLY)

/**
 * brief - Default flags for applications steps. 
 * By default each application should have the following behaviour: 
 * 1. Can be run as standalone (useful for running steps from diag command shell) 
 * 2. Is disabled during init. 
 */
#define DEF_PROP_DIS (UTILEX_SEQ_STEP_F_STANDALONE_EN | UTILEX_SEQ_STEP_F_SOC_PROP_DIS)
/*
 * }
 */

/*
 * Structs and Enums:
 * {
 */

/**
 * \brief dnxc init parameters
 */
typedef struct
{
    int no_init;
    int no_deinit;
    int no_dump;
    int warmboot;
#if AGGRESSIVE_ALLOC_DEBUG_TESTING
    int is_resources_check;
    int loop_id;
#endif
} appl_dnxc_init_param_t;


/*
 * }
 */



/*
* Function Declarations:
* {
*/


/**
 * \brief - This function parses init params and starts the init and deinit sequence according to init params. 
 * 
 * \param [in] unit - Unit ID  
 * \param [in] params - Explict init params
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnxc_init_step_list_run(
    int unit,
    appl_dnxc_init_param_t *params);


/**
 * \brief - Run a certain step (step_id) within the list of steps
 * 
 * \param [in] unit - Unit ID
 * \param [in] appl_id - step id to run 
 * \param [in] appl_steps - list of application steps 
 * \param [in] is_forward - Run forward or backward  
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnxc_step_list_id_run(
    int unit, 
    int appl_id, 
    const appl_dnxc_init_step_t *appl_steps,
    int is_forward);

/**
 * \brief - Run a certain step (step_name) within the list of steps
 * 
 * \param [in] unit - Unit ID
 * \param [in] step_name - step id to run 
 * \param [in] appl_steps - list of application steps 
 * \param [in] is_forward - Run forward or backward  
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnxc_step_list_name_run(
    int unit, 
    char *appl_name, 
    const appl_dnxc_init_step_t *appl_steps,
    int is_forward);

/**
 * \brief - Main BCM SDK init function. perfroms the following operation: 
 *          1. Attach and create unit. 
 *          2. Init BCM module (bcm_init()) 
 * 
 * \param [in] unit - Unit ID 
 * \param [in] init_param - Init application parameters 
 *   
 * \return
 *   shr_error_e    
 *   
 * \remark
 *   * This function is shared both for DNX and DFE as the same init procedures should be invoked by both.
 * \see
 *   * None
 */
shr_error_e
appl_dnxc_sdk_init(
    int unit);

/**
 * \brief - Main BCM SDK deinit function. perfroms the following operation: 
 *          1. Deinit BCM module (bcm_detach())
 *          2. Destroy unit.
 * 
 * \param [in] unit - Unit ID  
 *   
 * \return
 *   shr_error_e    
 *   
 * \remark
 *   * This function is shared both for DNX and DFE as the same init procedures should be invoked by both.
 * \see
 *   * None
 */
shr_error_e
appl_dnxc_sdk_deinit(
    int unit);

/**
 * \brief - Get the list of application steps
 * 
 * \param [in] unit - Unit ID
 * \param [Out] appl_steps - list of application steps
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnxc_step_list_get(
    int unit, 
    const appl_dnxc_init_step_t **appl_steps);

/*
 * }
 */

/* } */
#endif
