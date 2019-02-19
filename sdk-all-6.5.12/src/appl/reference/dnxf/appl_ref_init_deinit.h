/** \file appl_ref_init_deinit.h
 *
 * init and deinit functions to be used by the INIT_DNX command.
 *
 * need to make minimal amount of steps before we are able to use call bcm_init
 *
 */

/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef FILE_APPL_REF_DNXF_INIT_DEINIT_H_INCLUDED
/* { */
#define FILE_APPL_REF_DNXF_INIT_DEINIT_H_INCLUDED

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!"
#endif

#include <appl/diag/diag.h>
#include <appl/reference/dnxc/appl_ref_init_utils.h>
#include <shared/utilex/utilex_seq.h>

/*
 * MACROs:
 * {
 */

/*
 * }
 */

/*
 * Structs and Enums:
 * {
 */

/*
 * }
 */

/*
 * Function Declarations:
 * {
 */

/**
 * \brief - Get the list of DNXF application steps
 * 
 * \param [in] unit - Unit ID
 * \param [Out] appl_steps - pointer to the list of application steps
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnxf_step_list_get(
    int unit,
    const appl_dnxc_init_step_t ** appl_steps);

/**
 * \brief - BCM Shell command to init/deinit DNX
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #
 *   \param [in] args - arguments recieved to parse over.
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   cmd_result_t 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
cmd_result_t cmd_dnxf_init_dnx(
    int unit,
    args_t * args);

/**
 * \brief Describes the IDs for the different steps and 
 *        sub-steps of the DNX APPL.
 */
typedef enum
{
    /**
     * Invalid Value, used to indicate to APIs that this input is
     * not relevant
     */
    APPL_DNXF_STEP_INVALID_STEP = UTILEX_SEQ_STEP_INVALID,
    /**
     * SDK init (attach and bcm_init()) 
     */
    APPL_DNXF_STEP_SDK,
    /**
     * Reference applications 
     */
    APPL_DNXF_STEP_APPS,
    /**
     * Parse device params
     */
    APPL_DNXF_STEP_DEVICE_PARAMS,
    /**
     * STK init
     */
    APPL_DNXF_STEP_STK_INIT,
    /*
     * Interrupt init
     */
    APPL_DNXF_STEP_INTR_INIT,

    APPL_DNXF_STEP_LAST_STEP = UTILEX_SEQ_STEP_LAST
} appl_dnxf_step_id_e;

/*
 * }
 */

/* } */
#endif
