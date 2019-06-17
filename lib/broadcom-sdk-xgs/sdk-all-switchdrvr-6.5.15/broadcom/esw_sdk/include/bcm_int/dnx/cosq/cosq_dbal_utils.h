/** \file cosq_dbal_utils.h
 * $Id$
 * 
 * Generic DBAL access functionality
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _SRC_BCM_DNX_COSQ_DBAL_UITILS_H_INCLUDED_
/** { */
#define _SRC_BCM_DNX_COSQ_DBAL_UITILS_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <bcm/cosq.h>
#include <soc/dnx/dbal/dbal.h>

typedef struct
{
    dbal_fields_e id;
    uint32 value;
} dnx_cosq_dbal_field_t;

/**
 * \brief - set a value to dbal table entry
 *
 * \param [in] unit - unit index
 * \param [in] table_id - dbal table ID
 * \param [in] nof_keys - number of keys in the dbal table
 * \param [in] key - array of keys (id and value)
 * \param [in] result - dbal result field (id and value)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
shr_error_e dnx_cosq_dbal_entry_set(
    int unit,
    dbal_tables_e table_id,
    int nof_keys,
    dnx_cosq_dbal_field_t key[],
    dnx_cosq_dbal_field_t * result);

/**
 * \brief - get a value from dbal table entry
 *
 * \param [in] unit - unit index
 * \param [in] table_id - dbal table ID
 * \param [in] nof_keys - number of keys in the dbal table
 * \param [in] key - array of keys (id and value)
 * \param [in,out] result - dbal result field (id and value)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
shr_error_e dnx_cosq_dbal_entry_get(
    int unit,
    dbal_tables_e table_id,
    int nof_keys,
    dnx_cosq_dbal_field_t key[],
    dnx_cosq_dbal_field_t * result);

/**
 * \brief - return TRUE iff a key exists in the table
 *
 * \param [in] unit - unit index
 * \param [in] table_id - dbal table ID
 * \param [in] key_field_id - ID of key field
 * \param [out] exists - does key  exist in the table
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
shr_error_e dnx_cosq_dbal_key_exists_in_table(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e key_field_id,
    int *exists);

#endif
