/*! \file bcm_int/dnx/aacl/aacl_in_intf.h
 *
 * Internal DNX AACL APIs
 *
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _AACL_IN_INTF_H_INCLUDED__
/*
 * {
 */
#define _AACL_IN_INTF_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm/types.h>
/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/** Maximum number of group ids supported */
#define AACL_MAX_GROUP_ID 3

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
 * TYPEDEFs
 * {
 */


/** Maximum number of key fields supported */
#define AACL_MAX_NUM_OF_FILEDS 25

typedef enum
{
    AACL_TCAM_COMPRESSED_ENTRY,
    AACL_TCAM_OVERFLOW_ENTRY
} aacl_tcam_entry_type;

typedef enum
{
    AACL_LPM_INDEX1_ENTRY,
    AACL_LPM_INDEX2_ENTRY
} aacl_lpm_entry_type;

typedef shr_error_e(
    *aacl_out_tcam_entry_add) (
    int unit,
    int group_id,
    aacl_tcam_entry_type entry_type,
    uint32 *key,
    uint32 *mask,
    int priority,
    uint32 *payload,
    int payload_size,
    uint32 *entry_id);

typedef int (
    *aacl_out_tcam_entry_delete) (
    int unit,
    int group_id,
    aacl_tcam_entry_type entry_type,
    uint32 *key,
    uint32 *mask,
    uint32 entry_id);

typedef int (
    *aacl_out_tcam_entry_payload_update) (
    int unit,
    int group_id,
    aacl_tcam_entry_type entry_type,
    uint32 *key,
    uint32 *mask,
    uint32 *payload,
    int payload_size,
    uint32 entry_id);

typedef int (
    *aacl_out_lpm_entry_add) (
    int unit,
    int group_id,
    aacl_lpm_entry_type entry_type,
    uint32 *prefix,
    int prefix_length,
    uint32 *payload,
    int payload_size);

typedef int (
    *aacl_out_lpm_entry_delete) (
    int unit,
    int group_id,
    aacl_lpm_entry_type entry_type,
    uint32 *prefix,
    int prefix_length);

typedef int (
    *aacl_out_lpm_entry_payload_update) (
    int unit,
    int group_id,
    aacl_lpm_entry_type entry_type,
    uint32 *prefix,
    int prefix_length,
    uint32 *payload,
    int payload_size);

typedef struct
{
    aacl_out_tcam_entry_add tcam_entry_add;
    aacl_out_lpm_entry_add lpm_entry_add;
    aacl_out_tcam_entry_delete tcam_entry_delete;
    aacl_out_lpm_entry_delete lpm_entry_delete;
    aacl_out_tcam_entry_payload_update tcam_entry_payload_update;
    aacl_out_lpm_entry_payload_update lpm_entry_payload_update;
} aacl_mngr_out_intf_cb;

/** Structure to bookkeep the dbal table id for TCAM,LPM tables per group */
typedef struct aacl_out_dbal_info
{
    dbal_tables_e compressed_dbal_id;
    dbal_tables_e overflow_dbal_id;
    dbal_tables_e index1_dbal_id;
    dbal_tables_e index2_dbal_id;
} aacl_out_intf_dbal_table_info_t;

aacl_out_intf_dbal_table_info_t aacl_group_to_dbal_info[BCM_MAX_NUM_UNITS][AACL_MAX_GROUP_ID];
/************* Move to AACL_MNGR CODE INCLUDE FILES (END) */
/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

/**
* \brief
*   AACL Manager Init API, called from Init sequence.
*   \param [in] unit - Relevant unit.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_aacl_init(
    int unit);

/**
* \brief
*   AACL Manager DeInit API
*   \param [in] unit - Relevant unit.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_aacl_deinit(
    int unit);

/**
* \brief
*   AACL Manager Init API, called from Init sequence.
*   \param [in] unit - Relevant unit.
*   \param [out] is_enabled - Boolean indication if the AACL is enabled or not
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_aacl_is_enabled(
    int unit,
    uint8 *is_enabled);

/**
* \brief
*   This API creates AACL group.
*   Following this aacl_mngr_group_dbal_tables_connect() API has to be called
*   to associate all the DBAL table ids.
*   \param [in] unit - Relevant unit.
*   \param [in] group_id - AACL group id.
*   \param [in] group_name - Pointer to the AACL group name.
*   \param [in] flags - Flag specifies the group_id is input or output.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_aacl_group_create(
    int unit,
    uint32 *group_id,
    char *group_name,
    uint32 flags);

/**
* \brief
*   This API associates the corresponding DBAL table ids with AACL group.
*   \param [in] unit - Relevant unit.
*   \param [in] group_id - AACL group id.
*   \param [in] compressed_dbal_id - Compressed DBAL table id.
*   \param [in] overflow_dbal_id - Overflow DBAL table id.
*   \param [in] index1_dbal_id - Index-1 DBAL table id.
*   \param [in] index2_dbal_id - Index-2 DBAL table id.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_aacl_group_dbal_tables_connect(
    int unit,
    uint32 group_id,
    dbal_tables_e compressed_dbal_id,
    dbal_tables_e overflow_dbal_id,
    dbal_tables_e index1_dbal_id,
    dbal_tables_e index2_dbal_id);

/**
* \brief
*   This API is being calles in order to add an entry to a specific AACL group. This API uses AACL algo in order to
*   determine how to compress the entry to save tcam resources.
*   To complete the entry creation,
*   aacl_mngr_entry_key_field_masked_set should be called to set the entry field data
*   aacl_mngr_entry_key_field_range_set should be called to set the range field data (if present)
*   aacl_mngr_entry_result_field_set should be called to set the result field
*   aacl_mngr_entry_install should be called to commit the entry
*
*   \param [in] unit - Relevant unit.
*   \param [in] group_id - AACL group id.
*   \param [in] priority - Priority value of the ACL entry.
*   \param [out] entry_id - Entry id, which will be return to the user.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_aacl_entry_create(
    int unit,
    uint32 group_id,
    uint32 priority,
    uint32 *entry_id);

/**
* \brief
*   This API is used to set the field data of specific ACL entry.
*   \param [in] unit - Relevant unit.
*   \param [in] entry_id - ACL entry id.
*   \param [in] field_id - Field id.
*   \param [in] field_value - Pointer to the field data.
*   \param [in] field_mask - Pointer to the filed mask.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_aacl_entry_key_field_masked_set(
    int unit,
    uint32 entry_id,
    dbal_fields_e field_id,
    uint32 *field_value,
    uint32 *field_mask);

/**
* \brief
*   This API is used to set the range fields of specific ACL entry.
*   \param [in] unit - Relevant unit.
*   \param [in] entry_id - ACL entry id.
*   \param [in] field_id - Range field id.
*   \param [in] min_value - Min range value.
*   \param [in] max_value - Max range value.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_aacl_entry_key_field_range_set(
    int unit,
    uint32 entry_id,
    dbal_fields_e field_id,
    uint32 min_value,
    uint32 max_value);

/**
* \brief
*   This API is used to set result value for the specific ACL entry.
*   \param [in] unit - Relevant unit.
*   \param [in] entry_id - ACL entry id.
*   \param [in] field_id - Result field id.
*   \param [in] result_value - Pointer to the result value.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_aacl_entry_result_field_set(
    int unit,
    uint32 entry_id,
    dbal_fields_e field_id,
    uint32 *result_value);

/**
* \brief
*   This API is used to commit the last created entry (specific to incremental mode).
*   \param [in] unit - Relevant unit.
*   \param [in] entry_id - ACL entry id.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_aacl_entry_install(
    int unit,
    uint32 entry_id);

/**
* \brief
*   This API is used to collect the ACL entries(specific to batch mode).
*   The sub-sequent aacl_mngr_entry_install() calls are ignored and to commit these collected
*   entries aacl_mngr_commit() should be called.
*   \param [in] unit - Relevant unit.
*   \param [in] group_id - AACL group id.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_aacl_batch_collect(
    int unit,
    uint32 group_id);

/**
* \brief
*   This API is used to commit all the collected ACL entries (specific to batch mode)
*   in the same order it got created.
*   \param [in] unit - Relevant unit.
*   \param [in] group_id - AACL group id.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_aacl_batch_commit(
    int unit,
    uint32 group_id);

#endif/*_AACL_IN_INTF_H_INCLUDED__*/
