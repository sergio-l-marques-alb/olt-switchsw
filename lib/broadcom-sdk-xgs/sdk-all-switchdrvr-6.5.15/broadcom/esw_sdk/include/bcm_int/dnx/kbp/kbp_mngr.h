/*! \file bcm_int/dnx/kbp/kbp_mngr.h
 *
 * Internal DNX KBP APIs
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _KBP_MNGR_H_INCLUDED__
/*
 * {
 */
#define _KBP_MNGR_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_elk.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/swstate/auto_generated/access/kbp_fwd_tcam_access_mapper_access.h>
#include <bcm_int/dnx/kbp/kbp_connectivity.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#define KBP_ACCESS kbp_sw_state

#if defined(INCLUDE_KBP)

/** KBP is configured in dual port mode by default in SDK */
#define DNX_KBP_MAX_NOF_OPCODES                 KBP_HW_MAX_LTR_DUAL_PORT

#define DNX_KBP_MAX_NOF_LOOKUPS                 KBP_HW_MAX_SEARCH_DB
#define DNX_KBP_MAX_NOF_RESULTS                 KBP_INSTRUCTION_MAX_RESULTS

#define DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP     KBP_MAX_NUM_OF_SEGMENTS_PER_KEY

#define DNX_KBP_MAX_KEY_LENGTH_IN_BITS          NLMDEV_MAX_KEY_LEN_IN_BITS
#define DNX_KBP_MAX_PAYLOAD_LENGTH_IN_BITS      NLMDEV_MAX_AD_LEN_IN_BITS

#define DNX_KBP_MAX_NOF_SMTS                    NLM_MAX_NUM_SMT

#define DNX_KBP_MIN_ACL_PRIORITY                KBP_HW_MINIMUM_PRIORITY

#else

/*
 * Below defines are mapped to KBPSDK defines.
 * For compatibility with non-KBP compilations they are redefined to 0 or 1.
 */
#define DNX_KBP_MAX_NOF_OPCODES                 1 /** Used as array initializer */

#define DNX_KBP_MAX_NOF_LOOKUPS                 1 /** Used as array initializer */
#define DNX_KBP_MAX_NOF_RESULTS                 0

#define DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP     1 /** Used as array initializer */

#define DNX_KBP_MAX_KEY_LENGTH_IN_BITS          0
#define DNX_KBP_MAX_PAYLOAD_LENGTH_IN_BITS      0

#define DNX_KBP_MAX_NOF_SMTS                    0

#define DNX_KBP_MIN_ACL_PRIORITY                0

#endif

#define DNX_KBP_OPCODE_MIN                      1 /** Min opcode Value (HW) */
#define DNX_KBP_OPCODE_MAX                      200 /** MAx opcode Value (HW) */
#define DNX_KBP_MAX_NOF_DBS                     256 /** No suitable define was found amongst the KBPSDK defines */
#define DNX_KBP_MAX_SEGMENT_LENGTH_IN_BYTES     16 /** No suitable define was found amongst the KBPSDK defines */
#define DNX_KBP_HIT_INDICATION_SIZE_IN_BITS     8 /** the size of the hitbit in the result buffer from the KBP */

#define DNX_KBP_MAX_KEY_LENGTH_IN_BYTES         BITS2BYTES(DNX_KBP_MAX_KEY_LENGTH_IN_BITS)
#define DNX_KBP_MAX_PAYLOAD_LENGTH_IN_BYTES     BITS2BYTES(DNX_KBP_MAX_PAYLOAD_LENGTH_IN_BITS)

#define DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES  64 /** Arbitrary size that should fit almost all field names */
#define DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_WORDS  BYTES2WORDS(DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES)

#define DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY (DNX_KBP_MAX_NOF_LOOKUPS * DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP)

/*
 * In SMT mode the opcode pool is divided between the two threads.
 * Offset is used to configure same context to different thread.
 */
#define DNX_KBP_SMT_INSTRUCTION_OFFSET          DNX_KBP_MAX_NOF_OPCODES


#define DNX_KBP_RESULT_SIZE_FWD                 64
#define DNX_KBP_RESULT_SIZE_RPF                 32

/** 171 taken from "kbp_field_types_definitions.xml" */
#define DNX_KBP_KEY_SEGMENT_SIZE                (BITS2WORDS(171))

/*
 * }
 */
/*
 * INVALID DEFINEs
 * {
 */
#define DNX_KBP_INVALID_LOOKUP_ID                 -1
#define DNX_KBP_INVALID_RESULT_ID                 -1
#define DNX_KBP_INVALID_OPCODE_ID                 -1
#define DNX_KBP_INVALID_SEGMENT_ID                -1
#define DNX_KBP_INVALID_SEGMENT_SIZE              -1
#define DNX_KBP_INVALID_DB_ID                     -1
/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */

#if defined(INCLUDE_KBP)
typedef struct
{
    struct kbp_allocator *dalloc_p;
    void *xpt_p;
    struct kbp_device *device_p;
    struct kbp_device *smt_p[DNX_KBP_MAX_NOF_SMTS];
    uint32 flags;
    enum kbp_device_type device_type;
    struct kbp_device_config device_config;
    dnx_kbp_user_data_t user_data;
} generic_kbp_app_data_t;

#else

typedef struct
{
    void *dummy_p;
} generic_kbp_app_data_t;

#endif

/** Used in FP to provide to KBP manager information on new ACL lookup */
typedef struct
{
    /** the result index in the KBP result (payload) buffer */
    uint8 result_index;

    /** offset from the start of the KBP result buffer, indicating the result location of the lookup */
    uint32 result_offset;

    /** the DBAL table ID related to the lookup */
    dbal_tables_e dbal_table_id;

    /** the number of relevant segments */
    uint32 nof_segments;

    /** the relevant key segments from the master key */
    uint8 key_segment_index[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP];

    /*
     * The result size in the result buffer (note, can be smaller than the result size in the DB).
     * 0 means to take the size from the DBAL table (thus it will be the same as in the DB).
     */
    uint32 result_size;

} kbp_opcode_lookup_info_t;

typedef struct
{
    dbal_tables_e table_id;
    uint8 lookup_id;
    char lookup_type_str[DBAL_MAX_STRING_LENGTH];
    dbal_printable_entry_t entry_print_info;
} kbp_printable_entry_t;

/*
 * struct that represent one segment in the master key. this struct is used to retrieve/update the master key max the Max number segments is DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP
 */
typedef struct
{
    uint32 nof_bytes; /** size in bytes; in KBP segments has to be byte alignment incase DNX_KBP_INVALID_SEGMENT_SIZE the segment is not valid*/
    char name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES];
} kbp_mngr_key_segment_t;

/*
 * struct that represents a FWD context and all of the ACL contexts that can be derived from it.
 */
typedef struct
{
    dbal_enum_value_field_fwd2_context_id_e fwd_context;
    uint8 nof_acl_contexts;
    uint8 acl_contexts[DNX_DATA_MAX_FIELD_KBP_MAX_ACL_CONTEXT_NUM];
} kbp_mngr_fwd_acl_context_mapping_t;

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

/**
* \brief
*   KBP Manager Init API, called from Init sequence. Performs KBP device initialization, and set all static applications
*   \param [in] unit - Relevant unit.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_init(
    int unit);

/**
* \brief
*   KBP Manager De-Init API, called from DeInit sequence. Performs KBP device de-initialization
*   \param [in] unit - Relevant unit.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_deinit(
    int unit);

/**
* \brief
*   For a given opcode ID provides all FWD context and their respective ACL contexts that use the opcode ID.
*   \param [in] unit - Relevant unit.
*   \param [in] opcode_id - The opcode ID.
*   \param [out] fwd_nof_contexts - The number of fwd contexts the use the opcode_id. The numbe rod elements in fwd_acl_mapping.
*   \param [out] fwd_acl_ctx_mapping - An array of FWD contexts and the ACL contexts that derive from them.
*                                      Contains DNX_DATA_MAX_FIELD_KBP_MAX_CONTEXT_NUM_FOR_ONE_APPTYPE elements.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_to_contexts_get(
    int unit,
    uint8 opcode_id,
    uint8 *fwd_nof_contexts,
    kbp_mngr_fwd_acl_context_mapping_t fwd_acl_ctx_mapping[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE]);

/**
* \brief
*   this API returns the KBP opcode and opcode anme from the FWD and RPF contexts. 
*   \param [in] unit - Relevant unit.
*   \param [in] fwd_context - context of the required info
*   \param [in] acl_context - context of the required info
*   \param [out] opcode_id - the relevant opcode ID
*   \param [out] opcode_name - string that represnts the opcde should be at least char DBAL_MAX_STRING_LENGTH
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_context_to_opcode_get(
    int unit,
    uint8 fwd_context,
    uint8 acl_context,
    uint8 *opcode_id,
    char *opcode_name);

/**
 * \brief
 *   Returns printable entry for visibility packet diagnostic
 *   \param [in] unit - Relevant unit.
 *   \param [in] flags - Flags for result parsing.
 *   \param [in] fwd_context - Forwarding context (only from IFWD2 stage).
 *   \param [in] acl_context - ACL context.
 *   \param [in] key_sig_value - The key signal for parsing the lookup key.
 *   \param [in] key_sig_size - number of fields in the key.
 *   \param [in] res_sig_value - The result signal for parsing the lookup results.
 *   \param [in] res_sig_size - number of fields in the results.
 *   \param [in] nof_print_info - number of returned printable entry infos
 *   \param [out] entry_print_info - The returned printable entry info, built by the key and result signals.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
shr_error_e kbp_mngr_opcode_printable_entry_get(
    int unit,
    uint32 flags,
    uint8 fwd_context,
    uint8 acl_context,
    uint32 *key_sig_value,
    int key_sig_size,
    uint32 *res_sig_value,
    int res_sig_size,
    uint8 *nof_print_info,
    kbp_printable_entry_t * entry_print_info);

/**
* \brief
*   this API takes the KBP SW information update the DB according to the valid opcodes and configures the the KBP HW,
*   once this API is called the device is ready to use.
*   \param [in] unit - Relevant unit.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_sync(
    int unit);

/**
* \brief
*   this API updates the KBP HW with the previously applied configurations during warmboot.
*   \param [in] unit - Relevant unit.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_wb_sync(
    int unit);

/**
* \brief
*   This API creates KBP dynamic table. Used for ACLs that are created after init. It takes the DBAL KBP SW
*   information and transform it to the KBP info. Updates the number of DBs in KBP.
*   \param [in] unit          - Relevant unit.
*   \param [in] dbal_table_id - the related table ID.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_db_create(
    int unit,
    dbal_tables_e dbal_table_id);

/**
 * \brief - updating the KBP status in the SW table.
 * \param [in] unit - Relevant unit.
 * \param [in] kbp_mngr_status - example: DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_BEFORE_INIT,
 *        DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_INIT_DONE, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_FWD_CONFIG_DONE,
 *        DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_CONFIG_UPDATED, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED
  \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e kbp_mngr_status_update(
    int unit,
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status);

/**
 * \brief - Get the KBP status from the SW table.
 * \param [in]  unit - Relevant unit.
 * \param [out] kbp_mngr_status - The returned status. Can be one of these: DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_BEFORE_INIT,
 *        DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_INIT_DONE, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_FWD_CONFIG_DONE,
 *        DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_CONFIG_UPDATED, DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED
  \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e kbp_mngr_status_get(
    int unit,
    dbal_enum_value_field_kbp_device_status_e * kbp_mngr_status);

/**
 * \brief - Get from the SW table if KBP IPv4 public is enabled.
 * \param [in]  unit - Relevant unit.
 * \param [out] enabled - The returned indication, TRUE or FALSE.
  \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e kbp_mngr_ipv4_public_enabled_get(
    int unit,
    uint8 * enabled);

/**
 * \brief - Get from the SW table if KBP IPv6 public is enabled.
 * \param [in]  unit - Relevant unit.
 * \param [out] enabled - The returned indication, TRUE or FALSE.
  \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e kbp_mngr_ipv6_public_enabled_get(
    int unit,
    uint8 * enabled);

/**
 * \brief - Get the KBP DB ID using given DBAL table name.
 * \param [in]  unit  - Relevant unit.
 * \param [in]  name  - The name of the DBAL table.
 * \param [out] db_id - The ID of the found KBP DB.
  \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e kbp_mngr_db_id_by_name_get(
    int unit,
    char *name,
    uint8 *db_id);

/**
 * \brief - Get the total result size of a given opcode.
 * \param [in]  unit - Relevant unit.
 * \param [in]  opcode_id - Relevant opcode ID.
 * \param [out] result_size - The returned total opcode result size.
  \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e kbp_mngr_opcode_total_result_size_get(
    int unit,
    uint8 opcode_id,
    uint32 *result_size);

/**
* \brief
*   Set the master key of an opcode using the provided segments.
*   This will replace all current master key segments.
*   The first 'nof_segments' will be set according to the provided segments.
*   All other segments will be set to invalid '0' values.
*   \param [in] unit          - Relevant unit.
*   \param [in] opcode_id     - ID of related opcode.
*   \param [in] nof_segments  - The number of segments to set.
*   \param [in] key_segments  - Array of segments to be set to the opcode master key.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_master_key_set(
    int unit,
    uint8 opcode_id,
    uint32 nof_segments,
    kbp_mngr_key_segment_t * key_segments);

/**
* \brief
*   This API retrieves the master key structure for specific KBP opcode.
*   Used by FP for updating the KBP opcode master key when attaching KBP group to context.
*   \param [in] unit              - Relevant unit.
*   \param [in] opcode_id         - ID of related opcode.
*   \param [out] nof_key_segments - The number of retrieved segments.
*   \param [out] key_segments     - Array of retrieved segments info each segment is used. the location of the array is
*          the location in the master key.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_master_key_get(
    int unit,
    uint32 opcode_id,
    uint32 *nof_key_segments,
    kbp_mngr_key_segment_t * key_segments);

/**
* \brief
*   This API used to add segments to the master key for specific KBP opcode. This API will add only the new key
*   segments. all the segments in the master key has to have unique names. the new segments will be added at the end of
*   the master key.
*   \param [in] unit              - Relevant unit.
*   \param [in] opcode_id         - ID of related opcode.
*   \param [in] nof_new_segments  - The number of new segments.
*   \param [in] key_segments      - Array of segments to be added to the opcode master key.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_master_key_segments_add(
    int unit,
    uint32 opcode_id,
    uint32 nof_new_segments,
    kbp_mngr_key_segment_t * key_segments);

/**
* \brief
*   This API adds a lookup to an existing opcode. The lookup ID has to be available (unused) in the opcode.
*   Used by FP when attaching KBP group to a context.
*   \param [in] unit         - Relevant unit.
*   \param [in] opcode_id    - The related opcode ID to update.
*   \param [in] lookup_info  - The lookup info to add.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_lookup_add(
    int unit,
    uint8 opcode_id,
    kbp_opcode_lookup_info_t * lookup_info);

/**
* \brief
*   This API retrieves a lookup from an existing opcode.
*   \param [in] unit         - Relevant unit.
*   \param [in] opcode_id    - The related opcode ID to get.
*   \param [out] lookup_info - The lookup to get; result_index need to be set.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_lookup_get(
    int unit,
    uint8 opcode_id,
    kbp_opcode_lookup_info_t * lookup_info);

/**
* \brief
*   This API clears a lookup from an existing opcode.
*   \param [in] unit         - Relevant unit.
*   \param [in] opcode_id    - The related opcode ID.
*   \param [in] result_id    - The related result to clear.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_result_clear(
    int unit,
    uint8 opcode_id,
    uint8 result_id);

/**
* \brief
*   This API clears all configurations done for an opcode.
*   \param [in] unit         - Relevant unit.
*   \param [in] opcode_id    - The related opcode ID.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_clear(
    int unit,
    uint8 opcode_id);

/**
* \brief
*   This API create a new opcode accroding to the source opcode. all the source opcode info (lookups and master key) is
*   duplicated (since it is a cascaded opcode, meaning that the ACL is done on top of all other FWD lookups). NOTES:
*   this API support only apptypes that connected to one FWC Ctx.
*   \param [in] unit         - Relevant unit.
*   \param [in] source_opcode_id    - the opcode ID to generate from it the new opcode. (cascaded from).
*   \param [in] opcode_name    - The new opcode name.
*   \param [out] acl_ctx    - an available fwd2 context that will be use for this opcode.
*   \param [out] new_opcode_id    - the new opcode ID.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_opcode_create(
    int unit,
    uint8 source_opcode_id,
    char *opcode_name,
    uint8 *acl_ctx,
    uint8 *new_opcode_id);

/**
* \brief
*   this API inits the kbp_mngr_key_segment_t structure to default (invalid) values
*   \param [in] unit      - Relevant unit.
*   \param [in] segment_p - This procedure loads the pointed memory by that structure, containing invalid values.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_mngr_key_segment_t_init(
    int unit,
    kbp_mngr_key_segment_t * segment_p);

/**
* \brief
*   this API inits the kbp_opcode_lookup_info_t structure to default (invalid) values
*   \param [in] unit          - Relevant unit.
*   \param [in] lookup_info_p - The pointer to structure to initialize.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e kbp_opcode_lookup_info_t_init(
    int unit,
    kbp_opcode_lookup_info_t * lookup_info_p);

/**
 * \brief - Indicates if SMT is enabled for the KBP device or not.
 * \param [in] unit - Relevant unit.
  \return
 *   TRUE is SMT is enabled, FALSE otherwise
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 kbp_mngr_smt_enabled(
    int unit);

/**
 * \brief
 *   init to KBP device according to the KBP mode.
 *   \param [in] unit - Relevant unit.
 * \return
 *   \retval errors if unexpected behavior. See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
shr_error_e dnx_kbp_device_init(
    int unit);

/**
 * \brief
 *  deinit to KBP device according to the KBP mode.
 *   \param [in] unit - Relevant unit.
 * \return
 *   \retval errors if unexpected behavior. See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
shr_error_e dnx_kbp_device_deinit(
    int unit);

/*
 * }
 */
#endif/*_KBP_MNGR_H_INCLUDED__*/
