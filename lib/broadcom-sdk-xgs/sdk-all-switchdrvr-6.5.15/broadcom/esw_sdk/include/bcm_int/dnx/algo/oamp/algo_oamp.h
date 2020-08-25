/**
 * \file algo_oamp.h Internal DNX OAMP Template Management
ent $Copyright: (c) 2018 Broadcom.
ent Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef ALGO_OAMP_H_INCLUDED
/*
 * { 
 */
#define ALGO_OAMP_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/oam.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

 /**
 * Resource name defines for algo oamp
 * \see
 * dnx_oam_init_templates
 * {
 */

#define DNX_ALGO_OAMP_2B_MAID_FULL_MEP_ID    "OAMP_2B_MAID_FULL_MEP_ID"
#define DNX_ALGO_OAMP_2B_MAID_SHORT_MEP_ID   "OAMP_2B_MAID_SHORT_MEP_ID"
#define DNX_ALGO_OAMP_ICC_MAID_FULL_MEP_ID   "OAMP_ICC_MAID_FULL_MEP_ID"
#define DNX_ALGO_OAMP_ICC_MAID_SHORT_MEP_ID  "OAMP_ICC_MAID_SHORT_MEP_ID"
#define DNX_ALGO_OAMP_EXTRA_MEP_ID           "OAMP_EXTRA_MEP_ID"

/*
 * Callback defines for the sw state resource manager.
 */
#define SW_STATE_ALGO_OAMP_2B_MAID_FULL_MEP_ID    algo_oamp_db.oamp_2b_maid_full_mep_id
#define SW_STATE_ALGO_OAMP_2B_MAID_SHORT_MEP_ID   algo_oamp_db.oamp_2b_maid_short_mep_id
#define SW_STATE_ALGO_OAMP_ICC_MAID_FULL_MEP_ID   algo_oamp_db.oamp_icc_maid_full_mep_id
#define SW_STATE_ALGO_OAMP_ICC_MAID_SHORT_MEP_ID  algo_oamp_db.oamp_icc_maid_short_mep_id
#define SW_STATE_ALGO_OAMP_EXTRA_MEP_ID           algo_oamp_db.oamp_extra_mep_id

#define DNX_ALGO_OAMP_PP_PORT_2_SYS_PORT_TEMPLATE   "OAMP_PP_PORT_2_SYS_PORT"

#define DNX_ALGO_OAMP_MEP_DB_ENTRY           "OAMP_MEP_DB_ENTRY"

/** Number of sub entries that couldn't be used for endpoint in ICC MAID group in one MEP_DB entry */
#define DNX_OAM_NUM_OF_SHORT_ENTRIES_IN_MEP_DB_LINE 3

/** Structure used for MEP_DB memory type parameters */
typedef struct
{
    /**
     * Relevant for OAM only.
     *  Type of mep_id used for 2byte maid or icc maid group.
     *  MEP_ID for ICC MAID group should be multiple of 4.
     */
    uint8 is_2byte_maid;

    /**
     * Type of MEP ID:
     *          Short('0') - MEP ID resides below the threshold and allocate 1/4 MEP ID entry
     *          Full('1')  - MEP ID resides above the threshold and allocate full MEP ID entry
     */
    uint8 is_full;

    /**
     * MEP_ID Location: if it extra_pool it mean that MEP ID located in in banks 9-12.
     *
     */
    uint8 is_extra_pool;

} dnx_oamp_mep_db_memory_type_t;

/**
 * \brief - Printing callback for the PP Port profile template,
 *          which is used for mapping local port(pp port) to system port.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.
 * \param [in] print_cb_data - Pointer of the print callback data.
 *
 * \return
 *
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_oamp_pp_port_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - This function allocate mep id.
 *
 * \param [in]  unit - Number of hardware unit used.
 * \param [in]  flags - SW_STATE_ALGO_RES_ALLOCATE_WITH_ID flag in use. Allocate specific mep id.
 * \param [in]  mep_id_memory_type - requested memory type
 * \param [out] mep_id - MEP ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   dnx_*
 */
shr_error_e dnx_algo_oamp_mep_id_alloc(
    int unit,
    uint32 flags,
    dnx_oamp_mep_db_memory_type_t * mep_id_memory_type,
    uint32 *mep_id);

/**
 * \brief -This  function checks if mep id is allocated
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_id  - mep id that is checked.
 * \param [out] is_alloc - is allocated or not.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   dnx_*
 */
shr_error_e dnx_algo_oamp_mep_id_is_allocated(
    int unit,
    uint32 mep_id,
    uint8 *is_alloc);

/**
 * \brief -This  function release mep id from pool.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_id  - mep id that should be free
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   dnx_*
 */
shr_error_e dnx_algo_oamp_mep_id_dealloc(
    int unit,
    uint32 mep_id);

/**
 * \brief - initialize templates for all oamp profile types. 
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_oamp_init(
    int unit);

/**
 * \brief - Nothing to do here.
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_oamp_deinit(
    int unit);

/**
 * \brief - Utility function for Remote MEP creation
 *          This function deletes the new RMEP ID
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] rmep_id - ID of the remote endpoint.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oam_remote_endpoint_id_free(
    int unit,
    bcm_oam_endpoint_t rmep_id);

/**
 * \brief - Utility function for Remote MEP creation
 *          This function allocates the new RMEP ID
 *          and handles WITH_ID case as well.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] alloc_flags - flags variable to be used
 *             for resource allocation
 * \param [in,out] rmep_id - Resulting ID.  If ID is
 *        specified, this is an output argument.
 *        Otherwise, it's an input argument.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oam_remote_endpoint_id_alloc(
    int unit,
    int alloc_flags,
    bcm_oam_endpoint_t * rmep_id);

/**
 * \brief - Printing callback for the OAM/BFD TC/DP profile 
 *        templates.  The TC/DP profiles are used to construct
 *        the ITMH header for packets transmitted from OAM/BFD
 *        endpoints.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  The 
 *        OAM/BFD TC/DP profile structure dnx_oam_tc_dp_t is
 *        defined and described above.
 * \param [in] print_cb_data -
 *      Pointer of the print callback data.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_oam_itmh_priority_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - Printing callback for the OAM/BFD MPLS/PWE push 
 *        profile templates.  The push profiles are used to
 *        construct the packets transmitted from MPLS/PWE
 *        OAM/BFD endpoints.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  The 
 *        OAM/BFD MPLS/PWE PUSH profile structure
 *        dnx_oam_push_profile_t is defined and described above.
 * \param [in] print_cb_data -
 *      Pointer of the print callback data.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_oam_mpls_pwe_exp_ttl_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - Callback function for printing OAM punt profile template data
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Punt profile template data to be printed.
 * \param [in] print_cb_data - Pointer of the print callback data.
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_oam_punt_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - Utility function for reserving IDs for
 *          OAMP_MEP_DB entries.  This is used
 *          automatically by dbal and should not
 *          be used otherwise.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_index - Index to be used
 *        for resource allocation.
 */
shr_error_e dnx_algo_oamp_mep_entry_alloc(
    int unit,
    int *oamp_entry_index);

/**
 * \brief - Utility function for freeing IDs for
 *          OAMP_MEP_DB entries.  This is used
 *          automatically by dbal and should not
 *          be used otherwise.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_index - Index of resource
 *        to be freed.
 */
shr_error_e dnx_algo_oamp_mep_entry_dealloc(
    int unit,
    int oamp_entry_index);

/**
 * \brief - Callback function for printing OAM MEP profile template data
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - MEP profile template data to be printed.
 * \param [in] print_cb_data - Pointer of the print callback data.
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_algo_oam_oamp_mep_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/**
 * \brief - This function finds the next allocated short
 *          entry with sub-index 0
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in,out] icc_id_in_out - MEP ID.  As input, this is
 *        the starting point of the search.  As output, this is
 *        the result.
 * \param [out] entry_found - TRUE if search successful, FALSE
 *        otherwise
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_get_next_icc_entry(
    int unit,
    uint32 *icc_id_in_out,
    uint8 *entry_found);

/**
 * \brief - This function finds the next free short
 *          entry with sub-index 0
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] icc_id_out - resulting MEP ID.
 * \param [out] entry_not_found - FALSE if search successful, TRUE
 *        otherwise
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_oamp_mep_id_get_free_icc_id(
    int unit,
    uint32 *icc_id_out,
    uint8 *entry_not_found);
/*
 * } 
 */
#endif /* ALGO_OAMP_H_INCLUDED */
