/*
 * $Id: dbal_internal.h,v 1.13 Broadcom SDK $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef FLUSH_INTERNAL_H_INCLUDED
#define FLUSH_INTERNAL_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/l2/l2.h>

#define FLUSH_MACHINE_STATUS_IDLE 0

/*
 * Iterates over LEM DB
 */
#define MDB_LEM_ITER(unit,lem_id)    \
    for ((lem_id) = 0; (lem_id) < dnx_data_l2.general.lem_nof_dbs_get(unit); (lem_id)++)

/**
 * \brief - Reset the flush buffers that are used during the flush work when copying the entries
 *
 * \param [in] unit - unit id
 * \param [in] l2_action_type - Indicates the L2 action type, forwarding or learning
 *
 * \return
 *   void
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
void dbal_flush_reset_flush_work_buffers(
    int unit,
    dnx_l2_action_type_t l2_action_type);

/**
 * \brief - Do needed actions after the flush machine is done.
 * 
 * \param [in] unit - unit id
 * \param [in] l2_action_type - Indicates the L2 action type, forwarding or learning
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dbal_flush_end(
    int unit,
    dnx_l2_action_type_t l2_action_type);

/**
 * \brief - Set the flush rules and actions and start the flush machine
 *
 * \param [in] unit - unit id 
 * \param [in] bulk_commit - if set, start is issued from a bulk commit.
 * \param [in] hw_accelerated - if set, flush actions are HW accelerated (no DMA to CPU) 
 * \param [in] non_blocking - if set the API returns without waiting for the flush machine to finish. Wait is unset.
 * \param [in] l2_action_type - Indicates the L2 action type, forwarding or learning
 *
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dbal_flush_start(
    int unit,
    uint8 bulk_commit,
    uint8 hw_accelerated,
    uint8 non_blocking,
    dnx_l2_action_type_t l2_action_type);

/**
 * \brief - Return an entry from the DMA's host memory.
 * In case of flush, set the flush rules in the beginning (if needed) and reset on the last entry.
 *
 * \param [in] unit - unit id
 * \param [in,out] entry_handle - dbal handle to be filled with the results. The handle holds the channel id and flush rules.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dbal_flush_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);

/**
 * \brief - Init the flush buffer.
 * Used to store entries copied from the flush DMA host memory.
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dbal_flush_buffer_init(
    int unit);

/**
 * \brief - Deinit the flush buffer.
 *
 * \param [in] unit - unit id
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
void dbal_flush_buffer_deinit(
    int unit);

/**
 * \brief - Set the flush rules
 *
 * \param [in] unit - unit id
 * \param [in] app_db_id - app_db_id of the logical table in LEM to flush
 * \param [in] table_id - The table ID that the flush rules are aet for
 * \param [in] nof_rules - Number of rules to configure
 * \param [in] rules_info - pointer to rules info buffer, its size if according to nof_rules
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dbal_flush_rules_set(
    int unit,
    int app_db_id,
    dbal_tables_e table_id,
    int nof_rules,
    dbal_flush_shadow_info_t * rules_info);

/**
 * \brief - Clear flush DB rules and actions
 * 
 * \param [in] unit - unit id
 * \param [in] l2_action_type - Indicates the L2 action type (fwd/learn), in order to differentiate between the flush machines
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dbal_flush_db_clear(
    int unit,
    dnx_l2_action_type_t l2_action_type);

/**
 * \brief - Returns if the flash machine is running
 * 
 * \param [in] unit - unit id
 * \param [in] l2_action_type - Indicates the L2 action type, forwarding or learning
 * \param [in] flush_in_work - 1 if the flush is in work. 0 otherwise.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dbal_flush_in_work(
    int unit,
    dnx_l2_action_type_t l2_action_type,
    int *flush_in_work);

/**
 * \brief - Init the flush DB context
 * 
 * \param [in] unit - unit id
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dbal_flush_init(
    int unit);

#endif/*_DBAL_INCLUDED__*/
