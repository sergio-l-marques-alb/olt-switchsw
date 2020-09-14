/** \file sat_utils.c
 * $Id$
 *
 * SAT utilities for DNX.
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SAT

 /*
  * Include files.
  * {
  */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/dbal/dbal.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

 /*
  * Global and Static
  */
/*
 * }
 */

/**
* \brief
*       Configure Credit Count BackPressure
* \par
* \param [in] unit    - Relevant unit.
* \param [in] pipe_id - pipe_id
* \param [in] credit  - credit count backpressure
* \retval
*       Regular shr_error_e error handling
* \remark
* * None
*/
shr_error_e
dnx_sat_gtf_credit_config(
    int unit,
    int pipe_id,
    int credit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_CONFIG, &entry_handle_id));

    /** setting key */
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_SAT_PIPE_ID, pipe_id);

    /** setting result */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_BACKPRESSURE, INST_SINGLE, credit);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
