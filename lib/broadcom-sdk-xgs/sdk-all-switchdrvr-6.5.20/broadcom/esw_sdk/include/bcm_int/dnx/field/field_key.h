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
 * $Copyright: (c) 2020 Broadcom.
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

/* Starting offsets within keys for key templates.*/
/** Default starting offset in key for a lookup fg in key is 0 */
#define DNX_FIELD_KEY_LOOKUP_STARTING_OFFSET 0
/** Direct extraction has a starting offset of 0, that changes after attach.*/
#define DNX_FIELD_KEY_DE_STARTING_OFFSET 0
/** Hashing has a strating offset of 0*/
#define DNX_FIELD_KEY_HASH_STARTING_OFFSET 0
/** Compare allocates the msbs of the key, therefore the key-size is subtracted from single key size */
#define DNX_FIELD_KEY_COMPARE_STARTING_OFFSET(_unit) \
       (dnx_data_field.tcam.key_size_single_get(_unit) - \
        dnx_data_field.stage.stage_info_get(_unit, DNX_FIELD_STAGE_IPMF1)->compare_key_size)

/** State Table allocates the msbs of the key, therefore the key-size is subtracted from single key size */
#define DNX_FIELD_KEY_MDB_DT_STARTING_OFFSET(_unit) \
    (dnx_data_field.tcam.key_size_single_get(_unit) - dnx_data_field.map.key_size_get(_unit))
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
} dnx_field_key_attached_qual_info_t;

/**
* This structure hold the input parameters for dnx_field_key_attach function
* This is not part of field SW state
*/
typedef struct
{
    /** Field stage, for which stage the Field Group was created*/
    dnx_field_stage_e field_stage;
    /**
     *  Database type: The database may be:TCAM, Direct Table,
     *  or Direct Extraction.
     */
    dnx_field_group_type_e fg_type;

    /** Mapping of the qualifier inside the key*/
    dnx_field_key_template_t key_template;

    /** Specify which key size needs to be allocated by key module*/
    dnx_field_key_length_type_e key_length;
    /**
     * Access parameter for KBR (See, for example, DBAL table KLEAP_IPMF1_KBR_INFO (IPPC_PMF_KBR_PASS_1/))
     * Used from TCAM/MDB/EXEM - used to know how to perform lookup in the databases
     */
    dnx_field_app_db_id_t app_db_id;
    /**
     *This struct hold additional info for FFC config,
     * Used only for header type Qualifier to indicate in which layer number should be taken from
     * the number qualifier information is aligned to what held in key_template
     */
    dnx_field_qual_attach_info_t qual_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];

    /**
     * Compare attributes - in case the context has compare mode - which key/tcam result to compare
     * CMP1 : this FG will be compared against CMP1, internally SDK will try to allocate key B
     * CMP2 : this FG will be compared against CMP2, internally SDK will try to allocate key D*
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
    /**
      * Specify which ffc type should be configured (HEADER/META/LAYER-RECORD/etc)
      */
    dbal_fields_e ffc_type;
    /**
      * Specifies the offset of the qualifier data in the pbus that ffc should take the content from.
      */
    int offset;
    /**
      * Specifies the header number that the qualifier data would be taken from
      */
    int field_index;
    /**
      * Specifies the offset in the key that the qualifier data will be written to
      */
    uint32 key_offset;
    /**
      * Specifies the size of the data to be extracted from header and written into the key
      */
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
    * This structure hold the initial ffc information, including the ffc_id in IPMF1 and the hw instruction
    * for this FFC
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
    dnx_field_key_ffc_and_initial_ffc_info_t ffc_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL];
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
    * This structure holds the info respectfully to key_id per field group. For 80/160 bit key,
    * only the first element will be initialized. For the 320'b key, both elements will be initialized.
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
* For example, in Hash context we reserve key_I and key_J for hashing and
* we do not allow the usage of those key for tcam/de.
* Same about Compare contexts - they reserve ranges inside initial keys.
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
* \param [in] unit           - Device Id
* \param [in] qual_info_p  - Structure pointer to init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_qual_attach_info_t_init(
    int unit,
    dnx_field_qual_attach_info_t * qual_info_p);

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
*  Init Structure dnx_field_bit_range_t to invalid params
* \param [in] unit           - Device Id
* \param [in] bit_range_p    - Structure pointer to init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_key_bit_range_t_init(
    int unit,
    dnx_field_bit_range_t * bit_range_p);

/**
 * \brief
 *  Detach all context ID key information, i.e. deallocate all resources that were allocated for context ID to key at input
 * All relevant information about the Key can be read from the HW
 * \param [in] unit        - Device Id
 * \param [in] context_id  - Context ID to detach
 * \param [in] key_in_info_p -
 *   Key in info (see  dnx_field_key_attach_info_in_t for details)
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
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
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
 *  Detach PMF context per KEY for the special contexts (aka compare, hash, state table)
 * \param [in] unit            - Device ID
 * \param [in] stage           - Field stage
 * \param [in] context_id      - Context ID to perform the key detach for
 * \param [in] key_id_p        - Structure that holds the array of Key Id to deallocate
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
* \param [in] context_id       - Context ID
* \param [in] qual_type        - qualifier type
* \param [in] qual_info_p      - info from attach info.
* \param [out] dnx_qual_info_p - full output information for this qualifier.
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
    dnx_field_qual_attach_info_t * qual_info_p,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p);

/**
* \brief
*  Indicated if a qualifier can use up to one FFC (not including splitting between keys).
* \param [in] unit                - Device Id
* \param [in] field_stage         - For which field stage to configure HW
* \param [in] qual_type           - qualifier type
* \param [in] qual_info_p         - info from attach info.
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
    dnx_field_qual_attach_info_t * qual_info_p,
    uint8 *is_up_to_one_ffc_p);

/**
 * \brief
 *  This function will build key for context ID based on qual
 * \param [in] unit             - Device ID
 * \param [in] field_stage      - Field Stage (PMF1/2/3...)
 * \param [in] qual_types_p     - Key qualifiers
 * \param [in] starting_offset  - Offset to start from in key
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
    uint32 starting_offset,
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

shr_error_e dnx_field_key_qual_attach_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    dnx_field_qual_attach_info_t qual_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG]);

/**
 * \brief
 *  This function configures an FFC for the filling a zero padding segment.
 *
 * \param [in] unit           - Device Id
 * \param [in] opcode_id      - Opcode with the zero padding segment
 * \param [in] segment_id     - The zero padding segemnt index.
 * \param [in] segment_info_p - One of the field group and qualifier index that use the segment
 *                              (fill the zero padding segment) in the opcode. Useed to determine how the qualifier
 *                              looks. If the opcode isn't using zero padding, we take the segment info of another
 *                              opcode that does.
 * \param [in] is_zero_padding - Indicates if we perform an actual zero padding or just fill the FFC info so it
 *                               wouldn't write outsize the zero padding, as the KBR is controlled by the FWD context.
 * \param [in] opcode_id_that_uses_zero_padding - If the segment isn't zero padding for the opcode, we take another
 *                                                opcode where it is zero padded for the qualifier info.
 * \param [in] ffc_id         - The ffc_id to allocate (must be shared by all opcodes and all contexts.)
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_kbp_fwd_zero_padding_fill(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    int segment_id,
    dnx_field_group_kbp_segment_info_t * segment_info_p,
    int is_zero_padding,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id_that_uses_zero_padding,
    int ffc_id);

/**
 * \brief
 *  This function compares two qualifier arrays for KBP to check if they can be shared.
 *
 * \param [in] unit                  - Device Id
 * \param [in] nof_quals_1           - Number of qualifiers in first array.
 * \param [in] dnx_qual_1            - First qualifier array
 * \param [in] qual_attach_info_1    - First qualifier array's attach info
 * \param [in] nof_quals_2           - Number of qualifiers in second array.
 * \param [in] dnx_qual_2            - Second qualifier array
 * \param [in] qual_attach_info_2    - Second qualifier array's attach info
 * \param [out] fully_shareable_p          - Whether or not the qualifiers can be shared.
 * \param [out] partially_shareable_p      - Whether or not dnx_qual_2 can share part of dnx_qual_1.
 * \param [out] partial_sharing_lsb_offset_p - If we can do partial sharing, the offset from the lsb
 *                                             of dnx_qual_1 for sharing nof_quals_2.
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_kbp_acl_compare_qualifiers_for_sharing(
    int unit,
    int nof_quals_1,
    dnx_field_qual_t dnx_qual_1[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT],
    dnx_field_qual_attach_info_t qual_attach_info_1[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT],
    int nof_quals_2,
    dnx_field_qual_t dnx_qual_2[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT],
    dnx_field_qual_attach_info_t qual_attach_info_2[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT],
    int *fully_shareable_p,
    int *partially_shareable_p,
    int *partial_sharing_lsb_offset_p);

/**
 * \brief
 *  This function provides the key and location on key of a certain segement on KBP master key.
 *
 * \param [in] unit             - Device Id
 * \param [in] opcode_id        - The opcode id
 * \param [in] fwd_context      - The FWD context
 * \param [in] acl_context_id   - The ACL context
 * \param [in] seg_idx_on_master_key - The segemnt insex on the master key (from MSB to LSB)
 * \param [out] key_kbp_id_p    - The key on which the segment resides
 * \param [out] lsb_on_key_p    - The offset of the LSB of the segment (from the LSB of the key).
 * \param [out] size_on_key_p   - The part of the segment in bits on the same key as the LSB of the segment.
 *                                Can be smaller the segment size if the segment straddles multiple keys.
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_kbp_segment_position(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    uint8 fwd_context,
    dnx_field_context_t acl_context_id,
    int seg_idx_on_master_key,
    dbal_enum_value_field_kbp_kbr_idx_e * key_kbp_id_p,
    int *lsb_on_key_p,
    int *size_on_key_p);

/**
 * \brief
 *  This function provides the key and location on key of a certain FWD segement on KBP master key,
 *  and verifies it is the same for all fwd contexts
 *
 * \param [in] unit             - Device Id
 * \param [in] opcode_id        - The opcode id
 * \param [in] seg_idx_on_master_key - The segemnt insex on the master key (from MSB to LSB)
 * \param [out] key_kbp_id_p    - The key on which the segment resides
 * \param [out] lsb_on_key_p    - The offset of the LSB of the segment (from the LSB of the key).
 * \param [out] size_on_key_p   - The part of the segment in bits on the same key as the LSB of the segment.
 *                                Can be smaller the segment size if the segment straddles multiple keys.
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_kbp_fwd_segment_unified_position(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    int seg_idx_on_master_key,
    dbal_enum_value_field_kbp_kbr_idx_e * key_kbp_id_p,
    int *lsb_on_key_p,
    int *size_on_key_p);

/**
 * \brief
 *  This function provides the keys and the LSB psoition of the last key of a part of the master key,
 *  given offset from MSB on the amster key and size in bytes.
 *
 * \param [in] unit             - Device Id
 * \param [in] opcode_id        - The opcode id
 * \param [in] offset_on_master_from_msb - The offset from MSB on the master key.
 * \param [in] size_on_key      - The bits of the chunk we refer to.
 * \param [out] key_kbp_id_last_p - The last key (where the lsb sits on).
 * \param [out] keys_bmp_p      - The bitmap of the keys the chunk sits on.
 * \param [out] offset_on_last_key_p - The offset from lsb on the last key.
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_kbp_master_offset_to_key(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    int offset_on_master_from_msb,
    int size_on_key,
    dbal_enum_value_field_kbp_kbr_idx_e * key_kbp_id_last_p,
    uint32 *keys_bmp_p,
    int *offset_on_last_key_p);

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
 *  Configure the KBP opcode on the required contexts.
 * \param [in] unit        - Device Id
 * \param [in] opcode_id   - Opcode ID (AppType).
 * \param [in] fwd_context - FWD context ID.
 * \param [in] acl_context_id - ACL Context ID.
 * \param [in] kbp_opcode_info_p  - structure that holds KBP info, including the array of segments to configure.
 *                           The information given per segment is the fg_idx and pointers(indexes) into the fg_info.
 *                           We need those pointers in order to extract the attach information about a given qualifier.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_kbp_set(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    uint8 fwd_context,
    dnx_field_context_t acl_context_id,
    dnx_field_group_kbp_info_t * kbp_opcode_info_p);

/**
 * \brief
 *  This function converts the FWD2 (external stage) KBR form values 16-17 to
 *  KBP KBR values 2-3.
 *
 * \param [in] unit          - Device Id
 * \param [in] key_id        - Key Id
 * \param [out] kbp_kbr_id_p - Converted kbr id
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_field_key_fwd2_to_acl_key_id_convert
 */
shr_error_e dnx_field_key_fwd2_to_acl_key_id_convert(
    int unit,
    dbal_enum_value_field_field_key_e key_id,
    dbal_enum_value_field_kbp_kbr_idx_e * kbp_kbr_id_p);

/**
 * \brief
 *  This function converts the KBP KBR values 2-3 to FWD2 (external stage)
 *  KBR form values 16-17.
 *
 * \param [in] unit          - Device Id
 * \param [in] kbp_kbr_id        - KBP KBR Id
 * \param [out] key_id_p - Converted FWD2 kbr id
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_field_key_acl_to_fwd2_key_id_convert
 */
shr_error_e dnx_field_key_acl_to_fwd2_key_id_convert(
    int unit,
    dbal_enum_value_field_kbp_kbr_idx_e kbp_kbr_id,
    dbal_enum_value_field_field_key_e * key_id_p);

/**
 * \brief
 *  Configure the KBP ffc info get.
 * \param [in] unit        - Device Id
 * \param [in] fg_id   - Field group for which FFC info should be returned.
 * \param [in] opcode_id   - Opcode ID (AppType).
 * \param [in] fwd_context - FWD context ID.
 * \param [in] acl_context_id - ACL Context ID.
 * \param [out] group_ffc_info_p  - Structure that holds KBP FFC info.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *
 */
shr_error_e dnx_field_key_kbp_ffc_info_get(
    int unit,
    dnx_field_group_t fg_id,
    int opcode_id,
    uint8 fwd_context,
    dnx_field_context_t acl_context_id,
    dnx_field_key_group_ffc_info_t * group_ffc_info_p);

/**
 * \brief
 *  This function manages the available FFCs for a newly created ACL context by
 *  masking out the ffc's that are used by original FWD context.
 *
 * \param [in] unit           - Device Id
 * \param [in] base_opcode_id - The predefined opcode on which the new context is based.
 * \param [in] fwd_context_id - The FWD context that comes before the ACL context.
 * \param [in] acl_context_id - The newly created ACL cntext
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_field_key_ifwd2_acl_ffc_init
 */
shr_error_e dnx_field_key_ifwd2_acl_ffc_update_for_new_context(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e base_opcode_id,
    uint8 fwd_context_id,
    dnx_field_context_t acl_context_id);

/**
 * \brief
 *  This function manages the available for ACL context ffc by
 *  masking out the ffc's that are used by FWD context.
 *  The function goes over all ifwd2 contexts and checks which ffc's were marked as used by
 *  FWD context. Those ffc's are set as 'used' in the ffc resource manager, thus will never
 *  be allocated for the ACL context.
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_ifwd2_acl_ffc_init(
    int unit);

/**
 * \brief
 *  This function verifies that ACL Key resources were not used by other applications (such as Pemla).
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_key_ifwd2_acl_key_verify(
    int unit);

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

/* } */
/* } */
#endif
