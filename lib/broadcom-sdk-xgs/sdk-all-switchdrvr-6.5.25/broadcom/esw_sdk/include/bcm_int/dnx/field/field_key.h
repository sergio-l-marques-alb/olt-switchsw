/**
 * \file bcm_int/dnx/field/field_key.h
 *
 *
 * Field Processor definitions for Key for DNX
 *
 * Purpose:
 *     'Field Processor' (FP) definitions for KEY functions for other field modules for Packet Processor.
 *     Devices starting at DNX
 * Note:
 *      This file is added on top of the existing files in 'legacy' (ported
 *      from DPP).
 */
/*
 * $Id: $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef FIELD_FIELD_KEY_H_INCLUDED
/* { */
#define FIELD_FIELD_KEY_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif
/*
 * Include files
 * {
 */
#include <include/bcm_int/dnx/field/field.h>
#include <include/bcm_int/dnx/field/field_map.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_key_types.h>

/*
 * }
 */
/*
 * Defines
 * {
 */

/**
 * The maximum number of sub qualifiers a qualifier can split to.
 * For const qualifers, the number is the maximum number of consecutive ones without intermittent zeros in uint32 (input argument).
 * Splitting of non native qualifiers (such as header qualifier in iPMF2) to multiple stages isn't counted in
 * this define.
 * For other qualifers that could be the maximum parts in field map.
 */
#define DNX_FIELD_KEY_MAX_SUB_QUAL (MAX((DNX_FIELD_QAUL_MAX_NOF_MAPPINGS), (SAL_UINT32_NOF_BITS/2+1)))

/**
 * The maximum number of sub qualifiers a constant qualifier can split to.
 * It should accomodate all FFCs within maximum sized qualifier plus one for splitting between keys.
 * It should also accomodate the consecutive ones in const qualifier.
 */
#define DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL \
    (MAX((((DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE + DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC - 1) % \
           DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC) + 1), \
          ((DNX_FIELD_KEY_MAX_SUB_QUAL + 1) / 2)))

/*
 * }
 */
/*
 * Typedefs
 * {
 */
/**
 * Structure containing the information about a qualifier including the attach information.
 */
typedef struct
{
    /**
     * Qualifier type
     */
    dnx_field_qual_t qual_type;
    /** The size in bits of the qualifier*/
    uint32 size;
    /**
     * Offset from base - type of base depends on FCC type
     */
    int offset;
    /**
     * Layer ID, either absolute or relative to forwarding, depending in type
     */
    int index;
    /**
     * ffc type
     */
    dnx_field_ffc_type_e ffc_type;
    /**
     * FFC ranges that can be used by this qualifier
     */
    uint8 ranges;
    /**
     * The input type given in attach_info.
     */
    dnx_field_input_type_e input_type;
    /**
     * For iPMF2 only, whether we use the native metadata (Metadata 2).
     * If FALSE, The other fields in dnx_field_key_attached_qual_info_t refer to the iPMF1 FFC that sends the
     * data to the initial key.
     */
    int native_pbus;
    /**
     * LSB bit of the qualifier inside the key. either relative to the first key or the current key.
     */
    int offset_on_key;
} dnx_field_key_attached_qual_info_t;

/**
* This structure hold the input parameters for dnx_field_key_attach function
* This is not part of field SW state
*/
typedef struct
{

    dnx_field_stage_e field_stage;

    dnx_field_group_type_e fg_type;

    dnx_field_key_template_t key_template;

    dnx_field_key_length_type_e key_length_type;

    dnx_field_app_db_id_t app_db_id;

    dnx_field_qual_attach_info_t qual_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];

    /**
     * Compare attributes - in case the context has compare mode - which key/tcam result to compare
     * CMP1 : this FG will be compared against CMP1, internally SDK will try to allocate key B
     * CMP2 : this FG will be compared against CMP2, internally SDK will try to allocate key D
     */
    dnx_field_group_compare_id_e compare_id;

} dnx_field_key_attach_info_in_t;

/**
* This structure hold the input parameters for dnx_field_key_attach function specifically for
* direct extraction field groups.
* This is not part of field SW state
*/
typedef struct
{
    /**
     * Boolean flag. If 'FALSE' then all elements of this structure are meaningless
     * If TRUE, we expect to allocate a bitrange for the field group.
     */
    int bit_range_valid;
    /**
     * Boolean flag. If 'TRUE' then bit range was allocated while aligning the bit, specified
     * in 'bit_range_aligned_offset' below to bit_range_align_lsb_step.
     * See parameter 'do_align' in 'dnx_algo_field_key_id_bit_range_allocate()'.
     * This is used for FEM handling.
     */
    int bit_range_align_lsb;
    /**
     * The granularity/resolution in bits to align the lsb to, when using bit_range_align_lsb.
     */
    int bit_range_align_lsb_step;
    /**
     * Only meaningful if 'bit_range_align_lsb' is 'TRUE'. This is the bit, within allocated bit-range,
     * which has been aligned. Bit is identified by its index, starting from '0', which is the first (LS)
     * bit of the bit range. So, for example, a value of '2' would mean that the third bit has been aligned
     * (i.e., placed on, say, bit(32) of the key.
     * This is only relevant for FEM handling.
     */
    int bit_range_lsb_aligned_offset;
    /**
     * Boolean flag. If 'TRUE' then bit range was allocated while aligning the bit after the MSB
     * to bit_range_align_msb_step.
     * See parameter 'do_align' in 'dnx_algo_field_key_id_bit_range_allocate()'.
     * This is used for adding EFES with condition bits.
     */
    int bit_range_align_msb;
    /**
     * The granularity/resolution in bits to align the msb to, when using bit_range_align_msb.
     */
    int bit_range_align_msb_step;
} dnx_field_key_de_bit_allocation_info_t;

/**
* This structure hold the instruction parameters for the ffc hw configuration
* This is not part of field SW state
*/
typedef struct
{

    dbal_fields_e ffc_type;

    /*
     * Offset on the pbus
     */
    int offset;

    int field_index;
    /*
     * placment on the key it self
     */
    uint32 key_offset;

    uint32 size;
    /**
      * Specifies the 32'b instruction, as it appears in the hw
      */
    uint32 full_instruction;
} dnx_field_key_ffc_instruction_info_t;

/**
* This structure hold the information required for hw configuration of the ffc
* It holds the ffc id allocated for this qualifier and the instruction parameters for the ffc hw configuration
* This is not part of field SW state
*/
typedef struct
{
    /**
      * Specifies the ffc id allocated for the given qualifier (or part of qualifier)
      */
    uint32 ffc_id;
    /**
    * The key on which the FFC is located.
    */
    dbal_enum_value_field_field_key_e key_id;
    /**
      * Specifies the ffc instruction for the given ffc_id
      */
    dnx_field_key_ffc_instruction_info_t ffc_instruction;
} dnx_field_key_ffc_hw_info_t;

/**
* This structure hold the initial ffc information, including the ffc_id in IPMF1 and the hw instruction
* for this FFC
* This is not part of field SW state
*/
typedef struct
{
    /**
      * This structure hold the information required for hw configuration of the ffc
      */
    dnx_field_key_ffc_hw_info_t ffc;
    /**
      * Specifies the offset in the initial key-id for the allocated ffc
      */
    uint32 key_dest_offset;
} dnx_field_key_initial_ffc_info_t;

/**
* This structure holds the complete info for the ffc. It includes the ffc info, and initial ffc information
* in case it is relevant including the ffc_id and the instruction for all stages.
* This is not part of field SW state
*/
typedef struct
{
    /**
    * This structure hold the information required for hw configuration of the ffc
    * It holds the ffc id allocated for this qualifier and the hw instruction parameters for the ffc configuration
    */
    dnx_field_key_ffc_hw_info_t ffc;
    /**
    * This structure hold the initial ffc information, including the ffc_id in IPMF1
    */
    dnx_field_key_initial_ffc_info_t ffc_initial;
} dnx_field_key_ffc_and_initial_ffc_info_t;

/**
* This structure holds the complete info required for the ffc configuration per qualifier.
*/
typedef struct
{
    /**
    * The qualifier type that the information belongs to
    */
    dnx_field_qual_t qual_type;
    /**
    * This structure holds the complete info for the ffc. It includes the ffc info, and initial ffc information
    * in case it is relevant including the ffc_id and the instruction for all stages.
    */
    dnx_field_key_ffc_and_initial_ffc_info_t ffc_info[DNX_FIELD_KEY_MAX_NOF_FFC_IN_QUAL];
} dnx_field_key_qualifier_ffc_info_t;

/**
  * This structure holds the info respectfully to qualifier per field group.
*/
typedef struct
{
    /**
    * This structure holds the complete info required for the ffc configuration per qualifier.
    */
    dnx_field_key_qualifier_ffc_info_t qualifier_ffc_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
} dnx_field_key_group_key_info_t;

/**
* This structure holds the complete info required for the ffc configuration per field group.
*/
typedef struct
{
    /**
    * This structure holds the info respectfully to key_id per field group.
    */
    dnx_field_key_group_key_info_t key;
} dnx_field_key_group_ffc_info_t;
/*
 * }
 */

/*
* This structure holds the allowed ranges allowed for usage of the initial key per given context.
* Per Initial Key we state if this key valid (allowed for usage or resereved for other usages of the context),
* and the start and end offset inside the key.
* Example Compare contexts - they reserve ranges inside initial keys.
* Note: Filled with dnx_field_key_initial_ffc_sharing_range_get.
*/
typedef struct
{
    /*
     * States if Initial Key is valid
     * (allowed for usage as initial key (with shared FFCs) or resereved for other usages of the context)
     * TRUE means valid for initial key  with shared FFCs.
     */
    uint32 is_key_id_valid;
    /*
     * Start offset inside the key
     */
    uint32 offset_start;
    /*
     * End offset inside the key
     */
    uint32 offset_end;
} dnx_field_key_ffc_sharing_info_t;
/*
 * }
 */

/*
 * Prototypes
 * {
 */
/**
* \brief
*  Get the FFC id's bitmap, that were set in HW to build key
* \param [in] unit          -  Device ID
* \param [in] field_stage   -  Field Stage
* \param [in] context_id    - Context ID
* \param [in] key_id        - Key id of which FFC needs to be read
* \param [out] ffc_id   - Array FFC ID's that are configured in KBR HW for specific key
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_key_kbr_ffc_array_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC]);
/**
 * \brief
 *  Init the values of dnx_field_key_template_t type
 * \param [in] unit   - Device id
 * \param [in] key_template_p  - Key to init its value
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_template_t_init(
    int unit,
    dnx_field_key_template_t * key_template_p);

/**
* \brief
*  Init structure dnx_field_key_attach_info_in_t
* \param [in] unit           - Device Id
* \param [in] key_in_info_p  - pointer to structure dnx_field_key_attach_info_in_t
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_key_attach_info_in_t_init(
    int unit,
    dnx_field_key_attach_info_in_t * key_in_info_p);

/**
* \brief
*  Init Structure dnx_field_qual_attach_info_t to invalid params
* \param [in] unit               - Device Id
* \param [in] qual_attach_info_p - Structure pointer to init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_qual_attach_info_t_init(
    int unit,
    dnx_field_qual_attach_info_t * qual_attach_info_p);

/**
* \brief
*  Init Structure dnx_field_key_id_t to invalid params
* \param [in] unit           - Device Id
* \param [in] key_p          - Structure pointer to init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_key_id_t_init(
    int unit,
    dnx_field_key_id_t *key_p);

/**
 * \brief
 *  Detach all context ID key information, i.e. deallocate all resources that were allocated for context ID to key at input
 * All relevant information about the Key can be read from the HW
 * \param [in] unit        - Device Id
 * \param [in] field_stage - The stage of the context.
 * \param [in] context_id  - Context ID to detach
 * \param [in] fg_type    - The type of the field group.
 * \param [in] key_template_p - Key info.
 * \param [in] key_length_type - An enum indicating the size of the key.
 * \param [in] key_id_p    -
 *   Structure that holds the array of Key Id to deallocate
 *   Sub param of the key_id_p - bit_range     -
 *   Structure that holds all information regarding the bit-range allocated on key
 *   (specified in 'key_id', above) for this Context ID.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_detach(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_type_e fg_type,
    dnx_field_key_template_t * key_template_p,
    dnx_field_key_length_type_e key_length_type,
    dnx_field_key_id_t *key_id_p);

/**
 * \brief
 *  Attach context ID per KEY (I.e., allocate key per context
 *  and configure FFC and KBR accordingly).
 * \param [in] unit             -
 *   HW device ID
 * \param [in] context_id       -
 *   Context ID ID to perform the key attach for
 * \param [in] key_in_info_p    -
 *   Key in info (see  dnx_field_key_attach_info_in_t for details)
 * \param [in] bit_range_allocation_info_p    -
 *   Bit range allocation info. Only used for direct extraction.
 * \param [out] key_id_p        -
 *   Pointer type of dnx_field_key_id_t, holds an array of allocated keys
 *   needed as input to FES/FEM, and for FFC configuration
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_de_bit_allocation_info_t * bit_range_allocation_info_p,
    dnx_field_key_id_t *key_id_p);

/**
 * \brief
 *  Set bit on KBR on key
 * \param [in] unit           - Device ID
 * \param [in] field_stage    - Field stage
 * \param [in] context_id     - Context ID to set KBR for
 * \param [in] key_id         - Key to set a bit for
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_kbr_init(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id);

/**
 * \brief
 *  Detach PMF context per KEY for the special contexts (aka compare, hash, state table)
 * \param [in] unit            - Device ID
 * \param [in] stage           - Field stage
 * \param [in] context_id      - Context ID to perform the key detach for
 * \param [in,out] key_id_p        - Structure that holds the array of Key Id to deallocate. app_db_id is an out parameter.
 * \param [in] key_template_p  - Key Template
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_context_feature_key_detach(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p,
    dnx_field_key_template_t * key_template_p);

/**
 * \brief
 *  Attach PMF context per KEY for the special contexts (aka compare, hash, state table)
 * \param [in] unit               - Device ID
 * \param [in] context_id         - Context ID ID to perform the key attach for
 * \param [in] key_in_info_p      - First key in info look at dnx_field_key_attach_info_in_t
 * \param [in] key_id_p          - Pointer type of dnx_field_key_id_t, holds an array of allocated keys
                                    needed as input to FES/FEM, and for FFC configuration
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_context_feature_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p);

/**
 * \brief
 *  Detach PMF context per KEY for compare
 * \param [in] unit                    - Device ID
 * \param [in] context_id              - Context ID ID to perform the key attach for
 * \param [in] compare_key_info_p      - Key Id and key template
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_compare_detach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_key_info_t * compare_key_info_p);

/**
* \brief
*  Get the information for a given qualifier
* \param [in] unit             - Device Id
* \param [in] field_stage      - For which field stage to configure HW
* \param [in] context_id       - Context ID. Relevant only cor cadcaded input type.
* \param [in] qual_type        - qualifier type
* \param [in] offset_on_key    - The offset relative to the first key.
* \param [in] qual_attach_info_p - info from attach info.
* \param [out] nof_sub_quals_p - Number of valid elements in dnx_qual_info.
* \param [out] dnx_qual_info   - Full output information for this qualifier.
*                                Array of size DNX_FIELD_KEY_MAX_CONST_SUB_QUAL for when we split into sub qualifiers.
* \return
*   shr_error_e                - Error Type
* \remark
*   * None
* \see
*   * dnx_field_key_qual_info_uses_up_to_one_ffc
*/
shr_error_e dnx_field_key_qual_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_qual_t qual_type,
    int offset_on_key,
    dnx_field_qual_attach_info_t * qual_attach_info_p,
    int *nof_sub_quals_p,
    dnx_field_key_attached_qual_info_t dnx_qual_info[DNX_FIELD_KEY_MAX_SUB_QUAL]);

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
/* { */
/**
* \brief
*  Indicated if a qualifier can use up to one FFC (not including splitting between keys).
* \param [in] unit                - Device Id
* \param [in] field_stage         - For which field stage to configure HW
* \param [in] qual_type           - qualifier type
* \param [in] qual_attach_info_p  - info from attach info.
* \param [out] is_up_to_one_ffc_p - full output information for this qualifier.
* \return
*   shr_error_e                - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_key_qual_info_uses_up_to_one_ffc(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t qual_type,
    dnx_field_qual_attach_info_t * qual_attach_info_p,
    uint8 *is_up_to_one_ffc_p);
/* } */
#endif
/**
 * \brief
 *  This function will build key for context ID based on qual
 * \param [in] unit             - Device ID
 * \param [in] field_stage      - Field Stage (PMF1/2/3...)
 * \param [in] qual_types_p     - Key qualifiers
 * \param [out] key_template_p  - Hold the constructed key
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_template_create(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t * qual_types_p,
    dnx_field_key_template_t * key_template_p);

/**
 * \brief
 * Get information about an initial FFC by given initial key and context.
 * \param [in] unit             - Device ID
 * \param [in] context_id       - Context Id to get its info
 * \param [in] initial_key_id   - Initial key to get its info
 * \param [in] ffc_id           - FFC ID to get its info
 * \param [out] ipmf1_ffc_initial_p  - Pointer to dnx_field_key_initial_ffc_info_t to hold initial ffc info
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_single_initial_ffc_get(
    int unit,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e initial_key_id,
    uint32 ffc_id,
    dnx_field_key_initial_ffc_info_t * ipmf1_ffc_initial_p);

/**
 * \brief
 * Get information on the key that is attached to specified context
 * \param [in] unit              - Device ID
 * \param [in] field_stage       - Field Stage (PMF1/2/3...)
 * \param [in] context_id        - Context Id to get its info
 * \param [in] fg_type           - Type of field group
 * \param [in] key_id_p          - structure that holds an array of key ids for which the KBR will be updated
                                 - bit_range - is a subparam of key_id_p that holds all information regarding
                                   the bit-range allocated on key (specified in 'key_id', above) for this Context ID.
 * \param [in] key_template_p    - The key template, used to parse the FFCs.
 * \param [out] group_ffc_info_p - Pointer to dnx_field_key_group_ffc_info_t to hold full ffc info
 * \param [out] app_db_id_p      - The app_db_id used by the key
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_type_e fg_type,
    dnx_field_key_id_t *key_id_p,
    dnx_field_key_template_t * key_template_p,
    dnx_field_key_group_ffc_info_t * group_ffc_info_p,
    dnx_field_app_db_id_t * app_db_id_p);

/*
 * Procedures related to SW STATE for 'key'
 * {
 */
/**
 * \brief
 *   Initialize SW STATE for the whole 'KEY' module.
 * \param [in] unit     - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * See file field_key_alloc.xml where all definitions, related to \n
 *     SW STATE of key allocation, are stored. Root is 'dnx_field_keys_per_stage_allocation_sw'
 * \see
 *   dnx_field_keys_per_stage_allocation_t
 */
shr_error_e dnx_field_key_sw_state_init(
    int unit);
/*
 * }
 */

shr_error_e dnx_field_key_compare_mode_single_resources_reserve(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p,
    dnx_field_key_id_t *tcam_key_id_p);

shr_error_e dnx_field_key_compare_mode_single_resources_free(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p,
    dnx_field_key_id_t *tcam_key_id_p);

shr_error_e dnx_field_key_compare_mode_double_resources_reserve(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_1_p,
    dnx_field_key_id_t *initial_key_id_2_p);

shr_error_e dnx_field_key_compare_mode_double_resources_free(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_1_p,
    dnx_field_key_id_t *initial_key_id_2_p);

shr_error_e dnx_field_key_hash_resources_reserve(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id);

shr_error_e dnx_field_key_hash_resources_free(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p);

shr_error_e dnx_field_key_state_table_resources_reserve(
    int unit,
    dnx_field_stage_e context_field_stage,
    dnx_field_context_t context_id);

shr_error_e dnx_field_key_state_table_resources_free(
    int unit,
    dnx_field_stage_e context_field_stage,
    dnx_field_context_t context_id);

/**
 * \brief
 *  Find an FFC_ID available for a list of contexts.
 * \param [in] unit                     - Device Id
 * \param [in] field_stage              - Stage. At the moment only external stage is supported for this function.
 * \param [in] contexts_to_share_ffc    - Array of contexts.
 * \param [in] nof_context_to_share_ffc - number of contexts in contexts_to_share_ffc
 * \param [out] ffc_id_p                - FFC ID available in all contexts.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_find_common_available_ffc(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t contexts_to_share_ffc[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS],
    unsigned int nof_context_to_share_ffc,
    int *ffc_id_p);

/**
 * \brief
 *  This function configures the Extended L4 OPs FFCs with the given qual_type/qual_attach_info.
 *
 * \param [in] unit               - Device Id
 * \param [in] qual_type          - Qualifier type to configure FFC with
 * \param [in] ffc_id             - FFC ID to configure
 * \param [in] qual_attach_info_p - Qualifier attach info to configure FFC with
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_l4_ops_ffc_set(
    int unit,
    dnx_field_qual_t qual_type,
    uint32 ffc_id,
    dnx_field_qual_attach_info_t * qual_attach_info_p);

/**
 * \brief
 *  Configure the FFC's per context ID with given qualifier
 * \param [in] unit          - Device Id
 * \param [in] field_stage   - For which Field stage to Configure HW
 * \param [in] context_id    - Context ID to configure FFC's for
 * \param [in] dnx_qual_info_p   - Additional information for header qualifiers,
 *                                 to know from which layer the qualifier should be taken
 * \param [in] key_id         - Key Id for the the key being used for the TCAM lookup.
 * \param [in] use_specific_ffc_id   - Indicates whether to allocate a specific FFC_ID.
 *                                     If true, only one FFC qualifiers are supported.
 * \param [in] forced_ffc_id   - If use_specific_ffc_id is true, allocates this specific FFC ID.
*
* \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_key_ffc_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dbal_enum_value_field_field_key_e key_id,
    int use_specific_ffc_id,
    int forced_ffc_id);


/**
 * \brief
 *  Get the FFC's for a given qualifier, per context ID with given qualifier
 * \param [in] unit                  - Device Id
 * \param [in] field_stage           - For which Field stage to receive the HW information
 * \param [in] context_id            - Context ID
 * \param [in] key_id                - Key id to get its info
 * \param [in] dnx_qual_info_p       - Inforamtion about the qualifiers, to include type, size and loction on key.
 * \param [out] qualifier_ffc_info_p - Qualifier information
 * \param [in,out] output_ffc_index_p - start index for the ffc index. Used to concatenate the ffc's that belong to a qualifier
 *                                   in case the qualifier was split between different keys (for example tcam double key).
 *                                   In all other cases should be set to 0.
 * \return
 *   shr_error_e             - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_qual_ffc_get_internal(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p,
    uint32 *output_ffc_index_p);

/**
 * \brief
 *  Initialize an array of dnx_field_key_qualifier_ffc_info_t elements
 * \param [in] qualifier_ffc_info_p  - Pointer to an array of dnx_field_key_qualifier_ffc_info_t.
 * \param [in] num_of_elements       - Nimber of elements in the array qualifier_ffc_info_p.
 * \return
 *   shr_error_e             - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_field_key_qualifier_ffc_info_init(
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p,
    uint32 num_of_elements);

/**
 * \brief
 *  Verifications during init for key IDs.
 * \param [in] unit          - Device Id
 * \return
 *   shr_error_e             - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_init_verify(
    int unit);

/* } */
/* } */
#endif
