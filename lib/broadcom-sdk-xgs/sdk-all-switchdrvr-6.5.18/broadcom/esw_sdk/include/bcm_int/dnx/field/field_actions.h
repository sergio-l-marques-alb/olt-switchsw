/**
 * \file bcm_int/dnx/field/field_actions.h
 *
 *
 * Field Processor definitions for 'action - related' utilities for DNX
 *
 * Purpose:.
 *     'Field Processor' (FP) definitions for logical and physical (FES/FES) action-related operations.
 *     Devices starting at DNX
 */
/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef FIELD_FIELD_ACTIONS_H_INCLUDED
/* { */
#define FIELD_FIELD_ACTIONS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



/*
 * Include files
 * {
 */
#include <sal/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_actions_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_actions_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <bcm_int/dnx/field/field_entry.h>
/*
 * }
 */
/*
 * Defines
 * {
 */

/**
 * Value to load 'fes_instruction_info_type' to make 'fes_instruction_info_action_type' always valid.
 */
#define FES_INFO_VALID_TYPE_ALWAYS         1
/**
 * Value to load 'fes_instruction_info_type' to make 'fes_instruction_info_action_type' valid
 * depending on 'fes_instruction_info_polarity'.
 */
#define FES_INFO_VALID_TYPE_CONDITIONAL    0
#define FES_INFO_VALID_POLARITY_ASSERTED   1
#define FES_INFO_VALID_POLARITY_DEASSERTED 0
/**
 * Polarity indicated the value that the valid bit needs to be for the action to be valid.
 */
#define DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ONE (1)
#define DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ZERO (0)
/**
 * Number of bits in the 2 ms bits to set in the KEY_DATA
 * (of key_select) which will point to the selected action (in IPPC_FES_2ND_INSTRUCTION)
 */
#define DNX_FIELD_NOF_BITS_2MSB_SELECT 2
/**
 * Multiplier used to calculate the location, on the KEY, of the lowest bit of the two
 * bits assigned to the '2 ms bits' feature.
 * The location of the ls bit on the key may only be
 *   (DNX_FIELD_KEY_SELECT_BIT_GRANULARITY * n - DNX_FIELD_NOF_BITS_2MSB_SELECT).
 * where 'n' is between DNX_FIELD_MIN_LOCATIONS_OF_2MSB and DNX_FIELD_NUM_LOCATIONS_OF_2MSB
 * Based on the number of bit of difference between the key selects.
 */
#define DNX_FIELD_KEY_SELECT_BIT_GRANULARITY                  32
/**
 * \brief
 *  Maximum number of 2msb locations within a field group.
 *  
 */
#define DNX_FIELD_ACTIONS_NUM_LOCATIONS_OF_2MSB_ON_FG  \
    ((((DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) * (DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE)) \
      + (DNX_FIELD_KEY_SELECT_BIT_GRANULARITY + 1)) / (DNX_FIELD_KEY_SELECT_BIT_GRANULARITY))

/*
 * 'illegal' value in 'fem_key_select' field (of 'fem context') is used to indicate
 * that entry is inactive. HW is designed to actually follow that.
 */
#define DNX_FIELD_INVALID_FEM_KEY_SELECT     DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_INVALID_FEM_KEY_SELECT

/*
 * Verify that a 'fem context' (_context_id) entry is not already occupied
 * for specified FEM (_fem_id). If it is, exit with error code.
 */
#define DNX_FIELD_FEM_CONTEXT_NOT_OCCUPIED_VERIFY(_unit,_fem_id,_context_id)  \
{  \
    /*  \
     * Make sure FEM context is not occupied for this fem_id.  \
     * A FEM context is considered 'not occupied' if the 'fem_key_select' field \
     * is set to an 'illegal' value.  \
     */  \
    dnx_field_pmf_fem_context_entry_t _loc_dnx_field_pmf_fem_program_entry;  \
    /*   \
     */  \
    _loc_dnx_field_pmf_fem_program_entry.fem_id = _fem_id;  \
    _loc_dnx_field_pmf_fem_program_entry.context_id = _context_id;  \
    SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_get(_unit, &_loc_dnx_field_pmf_fem_program_entry));  \
    if (_loc_dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_key_select !=  \
        DNX_FIELD_INVALID_FEM_KEY_SELECT)  \
    {  \
        SHR_ERR_EXIT(_SHR_E_PARAM,  \
                     "This 'fem_id' (%d) on this fem context_id (%d) is already occupied (fes_key_select %d) while is should have been free. Quit. \n",  \
                     _fem_id, _context_id, _loc_dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_key_select);  \
    }  \
}
/*
 * Verify that a 'fem context' (_context_id) entry is not already occupied
 * for specified FEM (_fem_id). If it is, exit with error code.
 */
#define DNX_FIELD_FEM_CONTEXT_OCCUPIED_VERIFY(_unit,_fem_id,_context_id)  \
{  \
    /*  \
     * Make sure FEM context is occupied for this fem_id.  \
     * A FEM context is considered 'not occupied' if the 'fem_key_select' field \
     * is set to an 'illegal' value.  \
     */  \
    dnx_field_pmf_fem_context_entry_t _loc_dnx_field_pmf_fem_program_entry;  \
    /*   \
     */  \
    _loc_dnx_field_pmf_fem_program_entry.fem_id = _fem_id;  \
    _loc_dnx_field_pmf_fem_program_entry.context_id = _context_id;  \
    SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_get(_unit, &_loc_dnx_field_pmf_fem_program_entry));  \
    if (_loc_dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_key_select ==  \
        DNX_FIELD_INVALID_FEM_KEY_SELECT)  \
    {  \
        SHR_ERR_EXIT(_SHR_E_PARAM,  \
                     "This 'fem_id' (%d) on this fem context_id (%d) is free (fes_key_select %d) while is shoul have been occupied. Quit. \n",  \
                     _fem_id, _context_id, _loc_dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_key_select);  \
    }  \
}

/*
 * }
 */
/*
 * typedefs
 * {
 */

/**
 * Structure containing the configuration of FES_2ND_INSTRUCTION, not
 * including CHOSEN_MASK, for one FES2msb instruction.
 * see dnx_field_actions_fes_common_info_t
 * see dnx_field_actions_fes_info_t.
 * Use the 'packed' attribute to save space on large arrays which use
 * this structure (e.g., image of IPPC_FES_2ND_INSTRUCTION)
 */
typedef struct
{
    /*
     * HW identifier of 'action'.
     * 7 bits.
     * A value of invalid action type indicates 'empty entry'
     */
    dnx_field_action_type_t action_type;
    /*
     * Number of invalid MS bits.
     * This is the value placed in the "valid_bit" field of the fes instructions.
     * it is used to determines the number of bits to extract from Key to build of 'action'.
     * Despite its name, it actually refers to the number of invalid bits, the number of
     * MS bits to zero out in the FES action after the shift and before the mask is applied.
     * 5 bits
     */
    dnx_field_fes_valid_bits_t valid_bits;
    /*
     * Number of bits to shift from input 64 bits which are handed
     * over to the HW FES machine.
     * 6 bits
     */
    dnx_field_fes_shift_t shift;
    /*
     * If 'Type' is equal to 1 then this action is always valid
     * else
     *   If 'polarity' is '1' and the lsb of the shifted data is
     *   '1' then the action is valid. Otherwise, it is not
     *   If 'polarity' is '0' and the lsb of the shifted data is
     *   '0' then the action is valid. Otherwise, it is not
     * 'Type'     is 1 bit
     * 'Polarity' is 1 bit
     */
    dnx_field_fes_type_t type;
    dnx_field_fes_polarity_t polarity;
} __ATTRIBUTE_PACKED__ dnx_field_actions_fes_common_info_fes2msb_t;

/**
 * Structure containing the configuration of FES_2ND_INSTRUCTION, not
 * including CHOSEN_MASK, for one FES quartet.
 * see dnx_field_actions_fes_info_t.
 * Use the 'packed' attribute to save space on large arrays which use
 * this structure (e.g., image of IPPC_FES_2ND_INSTRUCTION)
 */
typedef struct
{
    /*
     * The information for each of the (4) FES2msb instructions within the FES quartet.
     */
    dnx_field_actions_fes_common_info_fes2msb_t fes2msb_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
} __ATTRIBUTE_PACKED__ dnx_field_actions_fes_common_info_t;

/**
 * Structure containing information known before allocation FES quartets and action masks,
 * for one FES2msb instruction.
 * dnx_field_actions_fes_alloc_info_t.
 * See dnx_field_actions_fes_info_t.
 */
typedef struct
{
    /*
     * The action mask is a mask that performs bitwise or on the action result.
     * 32 bits.
     */
    uint32 required_mask;
} __ATTRIBUTE_PACKED__ dnx_field_actions_fes_alloc_info_fes2msb_t;

/**
 * Structure containing information known before allocation FES quartets and action masks,
 * for one FES2msb instruction.
 * See dnx_field_actions_fes_info_t.
 */
typedef struct
{
    /*
     * The information for each of the (4) FES2msb instructions within the FES quartet.
     */
    dnx_field_actions_fes_alloc_info_fes2msb_t fes2msb_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
    /*
     * The priority given to each FES qaurtet.
     */
    dnx_field_action_priority_t priority;
} __ATTRIBUTE_PACKED__ dnx_field_actions_fes_alloc_info_t;

/**
 * Structure containing information used in writing FES configuration to FES_2ND_INSTRUCTION
 * and PMF_FES_PROGRAM, known after fes allocation.
 * See dnx_field_actions_fes_info_t.
 */
typedef struct
{
    /*
     * The value of the key select (FES_KEY_SELECT) to be issued to the multiplexer
     * which is correlated with all the values below.
     * Note that 'key_select' may vary as per stage:
     *  * dbal_enum_value_field_field_pmf_a_fes_key_select_e
     *  * dbal_enum_value_field_field_pmf_b_fes_key_select_e
     *  * dbal_enum_value_field_field_e_pmf_fes_key_select_e
     */
    dnx_field_fes_key_select_t key_select;
} __ATTRIBUTE_PACKED__ dnx_field_actions_fes_write_info_t;


typedef struct
{
    /*
     * The FES_2ND_INSTRUCTION configuration excluding CHOSEN_MASK, which points
     * to an action mask and may change according to allocation.
     */
    dnx_field_actions_fes_common_info_t common_info;
    /*
     * The fes information known before fes allocation
     */
    dnx_field_actions_fes_alloc_info_t alloc_info;
    /*
     * The fes information used for writing to HW after fes allocation
     */
    dnx_field_actions_fes_write_info_t write_info;
} __ATTRIBUTE_PACKED__ dnx_field_actions_fes_info_t;

/**
 * Structure containing the EFES configuration for a single 2msb combination for single EFES add.
 * See dnx_field_efes_action_add.
 */
typedef struct
{
    /*
     * Identifier of action, uses to retrieve action type.
     */
    dnx_field_action_t dnx_action;
    /*
     * The number of bits to take from the input.
     */
    uint8 size;
    /*
     * Offset on field group.
     */
    uint8 lsb;
    /*
     * Whether or not to use valid bit
     */
    uint8 dont_use_valid_bit;
    /*
     * The polarity of the valid bit (if dont_use_valid_bit==FALSE). one or two mean the value of the valid bit
     * for the action to take place.
     */
    uint8 valid_bit_polarity;
    /*
     * The or mask used.
     */
    dnx_field_fes_mask_t mask;
} dnx_field_actions_fes_single_add_conf_t;

/**
 * Structure containing the needed qual info for the qualifiers providing fes_data_2msb information for
 * creating an entry.
 * The information for the fes_data_2msb comes in the form of dataqualifiers. the number of the dataqualifiers
 * dictate what bit it provides information for, whilst the lsb of the qualifier data determines what bit we
 * expect to receive in that location in order for the fes_data_2msb feature to allow the action to occur.
 * e.g. if we have a dataqual 62 with lsb of 1 and dataqual 63 with lsb of we, only if the 62 bit of the fes
 * data is 1 and the 63 bit of the fes data is 0 the action would be valid.
 * Note that we do not use the qualifier mask.
 * In the future may use dnx_field_dataqual_t.
 */
typedef struct
{
    /*
     * The type of the qualifier
     */
    dnx_field_qual_t qual_type;
    /*
     * The lower bytes of the qualifier data (as we only need the lsb);
     */
    unsigned int val;
} dnx_field_dir_ext_quals_for_msb_bits_t;

/**
 * Structure containing information of a single FES2msb instruction.
 * Used for get commands.
 * See dnx_field_actions_fes_quartet_get_info_t.
 * See dnx_field_actions_fes_get_info_t.
 * See dnx_field_actions_context_fes_info_get().
 */
typedef struct
{
    /*
     * The parameters found in the FES instruction.
     * Action ID - The ID of the action to be performed (action type). Can be invalid.
     * Valid bitd - The number of MS bits to zero out
     *              (thus indicating how many bits we read from the input, not including the valid bit).
     * Shift - the bit where we begin to read the input (including the valid bit.)
     * Sype - If 1 we do not use valid bit. If zero we use valid bit.
     * Polarity - The meaning of the valid bit. If there is a valid bit (type=1),
     *            the action will happen only if he valid bit is equal to the polarity bit.
     * Chosen_mask - the index of the action mask to be used.
     */
    dnx_field_action_type_t action_type;
    dnx_field_fes_valid_bits_t valid_bits;
    dnx_field_fes_shift_t shift;
    dnx_field_fes_type_t type;
    dnx_field_fes_polarity_t polarity;
    dnx_field_fes_chosen_mask_t chosen_mask;
    /*
     * The value of the action mask that performs bitwise OR on the output. indicated by chosen_mask.
     */
    dnx_field_fes_mask_t mask;
} dnx_field_actions_fes_quartet_get_info_fes2msb_t;

/**
 * Structure containing information of a single FES quartet belonging to a specific context.
 * Used for get commands.
 * See dnx_field_actions_fes_context_info_t.
 * See dnx_field_actions_context_fes_info_get().
 */
typedef struct
{
    /*
     * The FES ID where this FES quartet is located.
     */
    dnx_field_fes_id_t fes_id;
    /*
     * The FES program ID where this FES quartet is located.
     */
    dnx_field_fes_pgm_id_t fes_pgm_id;
    /*
     * The action priority of this FES quartet for the context ID.
     */
    dnx_field_action_priority_t priority;
    /*
     * The field group that this FES quartet belongs to.
     */
    dnx_field_group_t fg_id;
    /*
     * The ordinal place (index) of the FES quartet within the field group.
     */
    uint8 place_in_fg;
    /*
     * The key select indicated where the input to the FES is taken from.
     */
    dnx_field_fes_key_select_t key_select;
    /*
     * The info about each of the 4 FES2msb instructions withing the FES quatret.
     */
    dnx_field_actions_fes_quartet_get_info_fes2msb_t fes2msb_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
} dnx_field_actions_fes_quartet_context_get_info_t;

/**
 * Structure containing information about the FES quartets allocated to a certain context ID.
 * Used for get commands.
 * See dnx_field_actions_context_fes_info_get().
 */
typedef struct
{
    /*
     * The number of valid FES quartets used by the context ID,
     * and therefore the number of valid elements in the array fes_quartets.
     */
    unsigned int nof_fes_quartets;
    /*
     * The info for each valid FES quartet used by the context ID.
     */
        dnx_field_actions_fes_quartet_context_get_info_t
        fes_quartets[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
} dnx_field_actions_fes_context_get_info_t;

/**
 * Structure containing information of a single FES quartet belonging to a specific context and field group.
 * Used for get commands.
 * See dnx_field_actions_fes_context_group_get_info_t.
 * See dnx_field_actions_context_fes_info_to_group_fes_info().
 */
typedef struct
{
    /*
     * The FES ID where this FES quartet is located.
     */
    dnx_field_fes_id_t fes_id;
    /*
     * The FES program ID where this FES quartet is located.
     */
    dnx_field_fes_pgm_id_t fes_pgm_id;
    /*
     * The action priority of this FES quartet for the context ID.
     */
    dnx_field_action_priority_t priority;
    /*
     * The key select indicated where the input to the FES is taken from.
     */
    dnx_field_fes_key_select_t key_select;
    /*
     * The info about each of the 4 FES2msb instructions withing the FES quatret.
     */
    dnx_field_actions_fes_quartet_get_info_fes2msb_t fes2msb_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
} dnx_field_actions_fes_quartet_context_group_get_info_t;

/**
 * Structure containing information about the FES quartets allocated to a certain context ID and field group.
 * Used for get commands.
 * See dnx_field_group_context_full_info_t.
 * See dnx_field_actions_context_fes_info_to_group_fes_info().
 */
typedef struct
{
    /*
     * The info for each valid FES quartet used by the field group for the context ID.
     * The order of the FES quartets are as their place in the field group.
     * Only for FES quartets added during group add.
     */
    dnx_field_actions_fes_quartet_context_group_get_info_t
        initial_fes_quartets[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    /*
     * The info for each valid FES quartet used by the field group for the context ID.
     * The order of the FES quartets are as their place in the field group.
     * Only for FES quartets added during EFES add.
     */
    dnx_field_actions_fes_quartet_context_group_get_info_t
        added_fes_quartets[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
} dnx_field_actions_fes_context_group_get_info_t;

/**
 * Structure containing information about single FES 2msb_info per FES program and context.
 */
typedef struct
{
    /*
     * The info about each of the 4 FES2msb instructions within the FES quartet.
     */
    dnx_field_actions_fes_quartet_get_info_fes2msb_t fes2msb_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
} dnx_field_actions_fes_pgm_ctx_info_t;

/**
 * Structure containing information about single FES Program.
 */
typedef struct
{
    /*
     * Stores the contexts for this FES-PGM. 
     */
    uint32 context_bmp[BITS2WORDS(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS)];

    /*
     * Store the FG for this FES-PGM. 
     */
    dnx_field_group_t field_group;

    /*
     * Store the used masks IDs for this FES-PGM. 
     */
    uint8 masks_id_used[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];

    /*
     * The info about each of the 4 FES2msb instructions per FES program and context.
     */
    dnx_field_actions_fes_pgm_ctx_info_t pgm_ctx_2msb_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
} dnx_field_actions_fes_pgm_info_t;

/**
 * Structure containing information per FES ID, like action masks and per program info.
 */
typedef struct
{
    /*
     * Store the action masks for this FES. 
     */
    dnx_field_fes_mask_t action_masks[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    /*
     * Store info per FES program for given FES ID. 
     */
    dnx_field_actions_fes_pgm_info_t fes_pgm_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS];
    /*
     * The ordinal place (index) of the FES quartet within the field group.
     */
    uint8 place_in_fg;
    /*
     * The key select indicated where the input to the FES is taken from.
     */
    dnx_field_fes_key_select_t key_select;

} dnx_field_actions_fes_id_get_info_t;

/**
 * Structure containing information for adding a single action using a fes.
 */
typedef struct
{
    /*
     * The action to be performed by the FES.
     */
    dnx_field_action_t dnx_action;
    /*
     * The indicator of the keys we read from.
     */
    dbal_enum_value_field_field_io_e field_io;
    /*
     * The location on the key, starting from the first key represented by field_io.
     */
    unsigned int lsb;
    /**
     * Indication on whether this action uses its ls bit
     * as 'valid' indication for this action:
     * If 'dont_use_valid_bit' is '1' then lsb is NOT used as 'valid' indication
     * If 'dont_use_valid_bit' is '0' then lsb IS used as 'valid' indication.
     */
    uint8 dont_use_valid_bit;
    /**
     * Indicate the priority per action that was configured for specific Field Group
     * the lower the number the higher the priority of the actions the higher FES id will be allocated
     */
    dnx_field_action_priority_t priority;
    /**
     * Indicate the polarity per action that was configured for specific Field Group, the meaning of the use_valid_bit.
     * Indicates whether condition bit can be based on 0 or 1. If there is a valid bit (use_valid_bit=1),
     * the action will happen only if the valid bit is equal to the polarity bit.
     */
    uint8 valid_bit_polarity;
} dnx_field_actions_single_fes_add_t;

/**
 * Structure containing information of a single FES quartet.
 * Used for get commands.
 * See dnx_field_actions_fes_info_t.
 * See dnx_field_actions_context_fes_info_get().
 */
typedef struct
{
    /*
     * The FES ID where this FES instruction is located.
     */
    dnx_field_fes_id_t fes_id;
    /*
     * The parameters found in the FES.
     * Action ID - The ID of the action to be performed (action type). Can be invalid.
     * Valid bitd - The number of MS bits to zero out
     *              (thus indicating how many bits we read from the input, not including the valid bit).
     * Shift - the bit where we begin to read the input (including the valid bit.)
     * Sype - If 1 we do not use valid bit. If zero we use valid bit.
     * Polarity - The meaning of the valid bit. If there is a valid bit (type=1),
     *            the action will happen only if he valid bit is equal to the polarity bit.
     * Chosen_mask - the index of the action mask to be used.
     * Mask - The value of the action mask that will perform bitwise or on the result of the FES.
     */
    dnx_field_action_type_t action_type;
    dnx_field_fes_valid_bits_t valid_bits;
    dnx_field_fes_shift_t shift;
    dnx_field_fes_type_t type;
    dnx_field_fes_polarity_t polarity;
    dnx_field_fes_chosen_mask_t chosen_mask;
    dnx_field_fes_mask_t mask;
} dnx_field_actions_fes_ace_instr_get_info_t;

/**
 * Structure containing information about the FES quartets allocated to a certain context ID.
 * Used for get commands.
 * See dnx_field_actions_context_fes_info_get().
 */
typedef struct
{
    /*
     * The number of valid FES instructions used by the ACE ID,
     * and therefore the number of valid elements in the array fes_instr.
     */
    unsigned int nof_fes_instr;
    /*
     * The info for each valid FES used by the ACE ID
     */
    dnx_field_actions_fes_ace_instr_get_info_t fes_instr[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT];
} dnx_field_actions_fes_ace_get_info_t;

/**
 * Structure containing information about actions as they appear in the SW state. Used for Field groups.
 * This structure exist here to allow functions to useit jointly without worrying about different sizes of arrays.
 * only used by dnx_field_group_fg_info_for_sw_t.
 * See dnx_field_actions_fg_payload_sw_info_t.
 * See dnx_field_group_fg_info_for_sw_t.
 */
typedef struct
{
    /*
     * The info about each action in the field group needed to construct an entry
     */
    dnx_field_action_in_group_info_t actions_on_payload_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
} dnx_field_actions_fg_payload_info_t;

/**
 * Structure containing information about actions as they appear in the SW state. Used for ACE groups.
 * This structure exist here to allow functions to be used jointly without worrying about different sizes of arrays.
 * Only used by dnx_field_actions_ace_payload_info_for_sw_t.
 * See dnx_field_actions_ace_payload_sw_info_t.
 * See dnx_field_actions_ace_payload_info_for_sw_t
 */
typedef struct
{
    /*
     * The info about each action in the ACE group needed to construct an entry
     */
    dnx_field_action_in_group_info_t actions_on_payload_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
} dnx_field_actions_ace_payload_info_t;

/*
 * }
 */

/**
 * Structure containing the fes_data 2msb information gleaned from the qualifiers.
 */
typedef struct
{
    /*
     * Tells whether or not 2msb qualifiers were found to give us information.
     * Redundant, can use qualifiers_width_for_2msb instead.
     */
    uint8 available;
    /*
     * The location of the first bit of 2msb.
     */
    unsigned int lsb;
    /*
     * The number of bits used starting from the lsb (in the future could support 0 or 1 bit, not just 2).
     */
    unsigned int bit_width;
    /*
     * The bits required for valid action, taken from the first bit of the qualifiers.
     */
    uint32 validity_condition;

} dnx_field_dir_ext_fes_data_2msb_information_t;

/**
 * Structure for mapping qualifiers to 2msb bits for the fes_data 2msb feature (which qualifier maps to which bit)
 */
typedef struct
{
    /*
     * Qualifier ID (This is supposed to be a qualifier assigned for the '2msb feature').
     */
    dnx_field_qual_t qualifier_type;
    /*
     * Location of bit, corresponding to 'qualifier_type', on FES_DATA
     */
    unsigned int bit_on_fes_data;
} dnx_field_actions_feature_2msb_qualifier_map_info_t;

/**
 * \brief
 *   Given 'field_io' which indicates the type of IO specified for a field group,
 *   get the encoded value of the TCAM results which are used by FEM.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] field_io -
 *   Enum of type 'dbal_enum_value_field_field_io_e'. IO type to
 *   convert. See 'dnx_field_group_context_full_info_t'
 * \param [out] replace_lsb_select_p -
 *   Pointer to element of type 'dbal_enum_value_field_field_fem_replace_lsb_select_e'.
 *   This the the value to write into HW to indicate TCAM output as specified in
 *   'field_io' above.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *    Dbal table 'FIELD_PMF_A_KEY_SELECT_N_FEM_PROG'
 *    Dbal field 'FIELD_FEM_REPLACE_LSB_SELECT'
 */
shr_error_e dnx_field_actions_io_to_replace_select(
    int unit,
    dbal_enum_value_field_field_io_e field_io,
    dbal_enum_value_field_field_fem_replace_lsb_select_e * replace_lsb_select_p);

/**
* \brief
*  Init single quarter from structure dnx_field_action_attach_info_t to invalid params
* \param [in] unit           - Device Id
* \param [in] action_info_p  - Structure pointer to init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_action_attach_info_single_t_init(
    int unit,
    dnx_field_action_attach_info_t * action_info_p);

/**
* \brief
*  Init two dimensional array of structure dnx_field_action_attach_info_t to invalid params
* \param [in] unit           - Device Id
* \param [in] action_info  - Structure pointer to init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_action_attach_info_t_init(
    int unit,
    dnx_field_action_attach_info_t action_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP]);

/**
* \brief
*  Init array (with DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES elements) of
*  structure dnx_field_actions_fes_single_add_conf_t.
* \param [in] unit              - Device Id
* \param [in] efes_action_info  - Pointer to an array of structures to init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_actions_fes_single_add_conf_t_init(
    int unit,
    dnx_field_actions_fes_single_add_conf_t efes_action_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES]);

/**
* \brief
*  Init the structure to default values dnx_field_action_in_group_info_t
* \param [in] unit      - Device ID
* \param [in] struct_p  - Pointer to structure to init the its values to default
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_action_in_group_info_t_init(
    int unit,
    dnx_field_action_in_group_info_t * struct_p);

/**
* \brief
*  Retrieve the size of the action, including valid bit if it uses it,
*  from a dnx_field_action_in_group_info_t structure.
* \param [in] unit           - Device ID
* \param [in] field_stage    - The thage for which we are looking for the size of the action.
* \param [in] action_p       - Pointer to a structure dnx_field_action_in_group_info_t.
*                              Only the 'dnx_action' and 'dont_use_valid_bit' fields are read.
*                              This function does not assume meaningful value in the 'lsb' field.
* \param [out] action_size_w_valid_p   - the size of the action, including the valid bit if it uses it.
* \param [out] action_size_wo_valid_p  - the size of the action, not including the valid bit.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_action_in_group_bit_size(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_action_in_group_info_t * action_p,
    unsigned int *action_size_w_valid_p,
    unsigned int *action_size_wo_valid_p);

/**
 * \brief
 *    Place input actions on specified line in 'actions table' so that they
 *    occupy the minimal number of bits.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] field_stage -
 *    dnx_field_stage_e. Stage for which this operation is carried out (for corresponding
 *    'actions table').
 * \param [in] max_nof_bits_on_payload -
 *    The maximum number of bits that the block used ("action_length_type") supports.
 *    For example, for TCAM it can be 32, 64 or 128 bits. Used for MSB alignment of the payload.
 * \param [in,out] actions_info -
 *    A structure with an array of structures of type 'dnx_field_action_in_group_info_t'.
 *    As input to the caller function it provides the type of actions and whether or not it they use valid bit.
 *    As output it provides the location of each action on the payload.
 *    Number of meaningful elements in the array is marked by the first entry
 *    whose 'action' is 'invalid' (DNX_FIELD_ACTION_INVALID), or
 *    DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP if all actions are valid.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_field_group_add
 */
shr_error_e dnx_field_actions_place_actions(
    int unit,
    dnx_field_stage_e field_stage,
    unsigned int max_nof_bits_on_payload,
    dnx_field_action_in_group_info_t actions_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP]);

/**
 * \brief
 *    Given, as input, placed actions on actions,
 *    get the EFESes that would load those actions from specified 'actions table'.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] field_stage -
 *    For which this operation is carried out (for corresponding 'actions table').
 * \param [in] field_io -
 *    Describes the location were the payload is written to, to find the necessary
 *    FES key select.
 * \param [in] actions_info -
 *    An array of structures the size of DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
 *    containing the actions we want to perform, their location on the payload and
 *    whether or not they use valid bit.
 * \param [in] template_shift -
 *    A bias added to the position of all actions in actions_info.
 * \param [in] action_attach_info
 *    Pointer to an array of dnx_field_action_attach_info_t structures with number of elements:
 *    DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
 *    Contains info per '2msb combination' and per action. This is per attach information,
 *    mainly action priority.
 * \param [out] fes_inst_info -
 *    Pointer to an array of structure of type dnx_field_actions_fes_info_t.
 *    The number of elements, for all 'stages' is:
 *        * DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
 *    Effectively, only part of the array is used, as per 'stage', as it is also limited by the numnber of feses:
 *    The number of elements in this array, for iPMF1/2, is at most:
 *        * DNX_DATA_MAX_FIELD_IPMF1_NOF_FES_INSTRUCTION_PER_PMF_PROGRAM
 *    And within the array, only DNX_DATA_MAX_FIELD_IPMF1_NOF_PROG_PER_FES elements are used.
 *        * DNX_DATA_MAX_FIELD_IPMF1_NOF_PROG_PER_FES
 *    The number of elements in this array, for iPMF3, is at most:
 *        * DNX_DATA_MAX_FIELD_IPMF3_NOF_FES_INSTRUCTION_PER_PMF_PROGRAM
 *    And within the array, only DNX_DATA_MAX_FIELD_IPMF3_NOF_PROG_PER_FES elements are used.
 *    The index of the array refers to the number of action in the corresponding field group (taken from 'info_p').
 *    This procedure loads pointed memory the required FES configuration that is needed for allocation and
 *    ultimately the information needed to be written to HW. only the 'common_info' and 'alloc_info' fields
 *    are changed.
 * \remark
 *   * None
 * \see
 *   * dnx_field_group_context_attach.
 *   * dnx_field_actions_calc_feses_single_efes_add
 */
shr_error_e dnx_field_actions_calc_feses(
    int unit,
    dnx_field_stage_e field_stage,
    dbal_enum_value_field_field_io_e field_io,
    dnx_field_action_in_group_info_t actions_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    int template_shift,
    dnx_field_action_attach_info_t action_attach_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP]);

/**
 * \brief
 *    Allocate and write to HW the FES configuration for requested actions
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] field_stage -
 *    For which of the PMF blocks the configuration is done (iPMF1/2/3, ePMF).
 * \param [in] fg_id -
 *    HW identifier of field group
 * \param [in] context_id -
 *    Context ID.
 * \param [in] is_nonshareable -
 *    If true, the algorithm should not allow FES sharing.
 * \param [in] is_post_attach -
 *    If true, the algorithm should consider this additional EFES to the field group.
 * \param [in,out] fes_inst_info
 *    Pointer to an array of dnx_field_actions_fes_info_t structures with
 *    the number of elements:
 *    DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
 *    Info is per action.
 *    Contains the FES configuration that the user-requested action requires. Is loaded
 *    by information from the allocation.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None.
 * \see
 *   * dnx_field_group_fes_config()
 */
shr_error_e dnx_field_actions_fes_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    int is_nonshareable,
    int is_post_attach,
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP]);
/**
 * \brief
 *    Allocate and write to HW the FES configuration for requested actions, for ACE ACR
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] ace_id -
 *    ACE result type, serves as context and field group for ACE.
 * \param [in,out] fes_inst_info
 *    Pointer to an array of dnx_field_actions_fes_common_info_fes2msb_t structures with
 *    the number of elements:
 *    DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
 *    Info is per action.
 *    Contains the FES configuration for each FES.
 *    Note that the structure's substructures contains four "fes2msb_info" but only uses the one with index 0.
 *    In the PMF EFESes, fes2msb ID indicates (together with FES ID and FES program ID) a single FES instruction,
 *    but in the ACE ACR EFESes, there is no 2msb feature and result type and FES ID indicate a single instruction.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None.
 * \see
 *    None.
 */
shr_error_e dnx_field_actions_fes_ace_set(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP]);

/**
 * \brief
 *    delete from HW and SW state the fes configuration for a specific field group for
 *    a specific context ID.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] field_stage -
 *    For which of the PMF blocks the configuration is done (iPMF1/2/3, ePMF).
 * \param [in] fg_id -
 *    HW identifier of field group
 * \param [in] context_id -
 *    Context ID.
 * \param [in] fes_id -
 *    If DNX_FIELD_EFES_ID_INVALID ignoring this parameter. Otherwise, only delete this sepcific FES ID.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None.
 * \see
 *   * dnx_field_actions_fes_set()
 *   * dnx_field_group_context_detach()
 */
shr_error_e dnx_field_actions_fes_detach(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_fes_id_t fes_id);

/**
 * \brief
 *    delete from HW the fes configuration for a specific ACE group in the ACE ACR EFES.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] ace_id -
 *    ACE group ID, also serves as context for the ACE ACR.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None.
 * \see
 *   * dnx_field_actions_fes_ace_set()
 *   * dnx_field_group_ace_delete().
 */
shr_error_e dnx_field_actions_fes_ace_detach(
    int unit,
    dnx_field_ace_id_t ace_id);

/**
* \brief
*    Gets the Information about The FES quartets allocated to the context ID.
* \param [in] unit -
*    Identifier of HW platform.
* \param [in] field_stage -
*    For which of the PMF blocks the configuration is done (iPMF1/2/3, ePMF).
* \param [in] context_id -
*    Cotnext ID.
* \param [out] context_fes_get_info_p -
*    Pointer to a structure dnx_field_actions_fes_context_get_info_t.
*    Loaded with information about the FES quartets used by the context ID.
* \return
*    \retval Zero - On success
*    \retval Error - Identifier as per shr_error_e
* \remark
*    None.
* \see
*   * dnx_field_group_context_get()
*/
shr_error_e dnx_field_actions_context_fes_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_actions_fes_context_get_info_t * context_fes_get_info_p);

/**
* \brief
*    Gets information about given FES ID.
* \param [in] unit -
*    Identifier of HW platform.
* \param [in] field_stage -
*    For which of the PMF blocks the configuration is done (iPMF1/2/3, ePMF).
* \param [in] fes_id -
*    FES ID.
* \param [out] fes_id_get_info_p -
*    Pointer to a structure dnx_field_actions_fes_id_get_info_t.
*    Loaded with information about contexts, field groups, fes programs and
*    chosen masks used for a given fes_id.
* \return
*    \retval Zero - On success
*    \retval Error - Identifier as per shr_error_e
* \remark
*    None.
* \see
*/
shr_error_e dnx_field_actions_fes_id_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    uint32 fes_id,
    dnx_field_actions_fes_id_get_info_t * fes_id_get_info_p);

/**
* \brief
*    Retrieves the information about FES configuration and priorities for a context ID and field group
*    from a structure containing the information for the entire context ID.
* \param [in] unit -
*    Identifier of HW platform.
* \param [in] fg_id -
*    Field group ID.
* \param [in] fes_context_info_p -
*    Pointer to a structure dnx_field_actions_fes_context_get_info_t.
*    Contains the information about FES configuration and priorities for a context ID.
* \param [out] fes_group_info_p -
*    Pointer to a structure dnx_field_actions_fes_context_group_get_info_t.
*    Loaded with the information about FES configuration and priorities for a context ID and field group.
* \return
*    \retval Zero - On success
*    \retval Error - Identifier as per shr_error_e
* \remark
*    None.
* \see
*   * dnx_field_group_context_get()
*/
shr_error_e dnx_field_actions_context_fes_info_to_group_fes_info(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_actions_fes_context_get_info_t * fes_context_info_p,
    dnx_field_actions_fes_context_group_get_info_t * fes_group_info_p);

/**
* \brief
*    Get the number of used EFES action masks in the ACE ACR.
* \param [in] unit -
*    Identifier of HW platform.
* \param [out] nof_masks -
*    The number of used masks for each FES ID.
* \return
*    \retval Zero - On success
*    \retval Error - Identifier as per shr_error_e
* \remark
*    None.
* \see
*    None.
*/
shr_error_e dnx_field_actions_ace_id_nof_mask_state_get(
    int unit,
    uint8 nof_masks[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT]);

/**
* \brief
*    Gets the Information about The FES instructions in the FESes with valid actions used by  the ACE ID.
* \param [in] unit -
*    Identifier of HW platform.
* \param [in] ace_id -
*    ACE ID.
* \param [out] ace_id_fes_get_info_p -
*    Pointer to a structure dnx_field_actions_fes_ace_get_info_t.
*    Loaded with information about the FES instructions used by the ACE ID.
* \return
*    \retval Zero - On success
*    \retval Error - Identifier as per shr_error_e
* \remark
*    None.
* \see
*    None.
*/
shr_error_e dnx_field_actions_ace_id_fes_info_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_actions_fes_ace_get_info_t * ace_id_fes_get_info_p);

/**
 * \brief
 *    Gets the Information about The FES instructions in the FESes with valid actions used by  the ACE ID.
 * \param [in] unit - Identifier of HW platform.
 * \param [in] fg_id - The field group.
 * \param [in] context_id - The context_id the field group it attached to.
 * \param [in] action_priority - The position of the EFES.
 * \param [in] use_condition - Indicating whether or not we use the 2MSB feature.
 * \param [in] efes_condition_msb - The offset on the field group of the MSB of the condition bits of the 2MSB,
 *                                  if use_condition is true
 * \param [in] efes_encoded_extraction - An array with DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES elements.
 *                                       Each element details the configuration for one of the 2msb combinations.
 *                                       If use_condition is false, we only read the first element.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None.
 * \see
 *    None.
 */
shr_error_e dnx_field_efes_action_add(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    bcm_field_action_priority_t action_priority,
    int use_condition,
    int efes_condition_msb,
    dnx_field_actions_fes_single_add_conf_t
    efes_encoded_extraction[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES]);

/**
 * \brief
 *    Gets the Information about The FES instructions in the FESes with valid actions used by  the ACE ID.
 * \param [in] unit - Identifier of HW platform.
 * \param [in] fg_id - The field group.
 * \param [in] context_id - The context_id the field group it attached to.
 * \param [in] action_priority - The position of the EFES.
 * \param [out] use_condition_p - Indicating whether or not we use the 2MSB feature.
 * \param [out] efes_condition_msb_p - The offset on the field group of the MSB of the condition bits of the 2MSB,
 *                                     if use_condition is true
 * \param [out] efes_encoded_extraction - An array with DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES elements.
 *                                        Each element details the configuration for one of the 2msb combinations.
 *                                        If use_condition is false, we only read the first element.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None.
 * \see
 *    None.
 */
shr_error_e dnx_field_efes_action_info_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    bcm_field_action_priority_t action_priority,
    int *use_condition_p,
    int *efes_condition_msb_p,
    dnx_field_actions_fes_single_add_conf_t
    efes_encoded_extraction[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES]);

/**
 * \brief
 *    Remove an EFES added using dnx_field_efes_action_add.
 * \param [in] unit - Identifier of HW platform.
 * \param [in] fg_id - The field group.
 * \param [in] context_id - The context_id the field group it attached to.
 * \param [in] action_priority - The position of the EFES. Used to identify the position of the EFES.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None.
 * \see
 *    None.
 */
shr_error_e dnx_field_efes_action_remove(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    bcm_field_action_priority_t action_priority);

/**
 * \brief
 *    Initialized an array of dnx_field_actions_fes_info_t with the number of elements:
 *    DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP.
 *    Zeroes the structures and then placed invalid indications.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] field_stage -
 *    For which of the PMF blocks the configuration is done (iPMF1/2/3, ePMF).
 * \param [out] fes_inst_info
 *    Pointer to an array of dnx_field_actions_fes_info_t structures with the number of elements
 *    DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT
 *    to be initialized.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None.
 * \see
 *   * dnx_field_actions_calc_feses()
 *   * dnx_field_group_fes_config()
 *   * dnx_field_dir_ext_fes_setup()
 */
shr_error_e dnx_field_actions_fes_info_t_init(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP]);
/*
 * Definitions related to 'FEM machine'
 * {
 */
/*
 * Defines
 * {
 */

/**
 * \brief
 *  If 'DNX_FIELD_INIT_FEM_TABLES_FAST' is set to a non-zero value then initialization
 *  of various FEM tables is faster but it directly accesses DBAL without using the standard
 *  'default' procedures. This is less modular.
 * \see dnx_field_init_fem_tables()
 *  
 */
#define DNX_FIELD_INIT_FEM_TABLES_FAST          0
/**
 * \brief
 *  Ordinal number of bit used for creating DNX_FIELD_FEM_MASK_FOR_MAP_TYPE.
 * \see DNX_FIELD_FEM_MASK_FOR_MAP_TYPE
 *  
 */
#define DNX_FIELD_FEM_BIT_FOR_MAP_TYPE          (dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_bits_in_fem_field_select - 1)
/**
 * \brief
 * Mask of the bit that is used to identify the type FIELD_FEM_BIT_VAL.
 *   If this bit is set to DNX_FIELD_FEM_MAP_TYPE_KEY_SELECT then
 *     FIELD_FEM_BIT_VAL is the location of the bit on the 'key select'
 *   If this bit is NOT set to DNX_FIELD_FEM_MAP_TYPE_KEY_SELECT then
 *     FIELD_FEM_BIT_VAL is programmed for either MAP_DATA or 
 * \remark
 * * See dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->log_nof_bits_in_fem_map_data_field
 * * See decsription of FIELD_FEM_BIT_VAL in, say, table FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_0_1_MAP
 *   in hl_acl_field_fem_definition.xml
 */
#define DNX_FIELD_FEM_MASK_FOR_MAP_TYPE       SAL_BIT(DNX_FIELD_FEM_BIT_FOR_MAP_TYPE)
/*
 * \brief
 * See DNX_FIELD_FEM_MASK_FOR_MAP_TYPE. If, when 'and'ing it with 'fem_bit_val',
 * we get DNX_FIELD_FEM_MAP_TYPE_KEY_SELECT then 'fem_bit_val' is encoded so
 * that the 5 LS bits are location on key-select.
 */
#define DNX_FIELD_FEM_MAP_TYPE_KEY_SELECT    0
/**
 * \brief
 * Mask of the bits that are used to identify the type FIELD_FEM_BIT_VAL.
 *   If these bits are set to DNX_FIELD_FEM_MASK_OF_MAP_DATA then
 *     FIELD_FEM_BIT_VAL indicates location of the bit on MAP_DATA
 * \remark
 * * See dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->log_nof_bits_in_fem_map_data_field
 * * See description of FIELD_FEM_BIT_VAL in, say, table FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_0_1_MAP
 *   in hl_acl_field_fem_definition.xml
 */
#define DNX_FIELD_FEM_MASK_FOR_MAP_DATA        \
    (SAL_UPTO_BIT(dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_bits_in_fem_field_select) & \
    SAL_FROM_BIT(dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->log_nof_bits_in_fem_map_data_field))
/**
 * \brief
 * Bit pattern which is used to identify FIELD_FEM_BIT_VAL as a value
 * of the index of a bit within field MAP_DATA on FEM_MAP_INDEX_TABLE
 * See, for example, FIELD_SELECT_MAP_0 and MAP_DATA (on registers file)
 * Note that the last two bits MUST be set to zero here. See
 * dnx_data_field.stage.stage_info_get_get(unit, DNX_FIELD_STAGE_IPMF1)->log_nof_bits_in_fem_map_data_field
 */
#define DNX_FIELD_FEM_MASK_OF_MAP_DATA        0x20
/**
 * \brief
 * Bit pattern which is used to identify FIELD_FEM_BIT_VAL as a value,
 * by itself, of the action bit
 * See, for example, FIELD_SELECT_MAP_0 and MAP_DATA (on registers file)
 * Note that the last bit MUST be set to zero here. See
 * DNX_FIELD_FEM_NUM_VALUES_ON_BIT_VALUE
 */
#define DNX_FIELD_FEM_MASK_OF_BIT_VALUE       0x30
/**
 * \brief
 * Number of values allowed when the DNX_FIELD_FEM_BIT_FORMAT_FROM_THIS_FIELD is
 * selected. (Actually, this is the number of values possible on 1 bit).
 * See 'dnx_field_fem_bit_format_e'
 */
#define DNX_FIELD_FEM_NUM_VALUES_ON_BIT_VALUE 2
/**
 * Defines for SW STATE
 * Shortcuts containing a few steps from root of SW STATE till indicated field.
 * See field_actions.xml
 * {
 */
#define FEM_INFO                     dnx_field_action_sw_db.fem_info
#define FEM_INFO_FG_ID_INFO          FEM_INFO.fg_id_info
#define FEM_INFO_FEM_ENCODED_ACTIONS FEM_INFO.fg_id_info.fem_encoded_actions
/*
 * }
 */
/**
 * This remark relates only to CMODEL on the stage where it is still impossible to
 * 'neutralize' a fem right on the 'context' line:
 * Identifier of 'FEM program' that is used as 'fallback' (NO ACTION) for
 * all 'FEM id's. 'FEM program's with higher index may be used by field groups.
 * See 'nof_bits_in_fem_programs' in 'dnx_field.xml'
 */
#define DEFAULT_FEM_PROGRAM         0
/*
 * The first 'FEM program' that may be used by any Field Group. The remaining
 * 'FEM program' is the 'default' one and is used as 'fallback' (NO ACTION) for
 * all 'FEM id's.
 * Since it is possible to 'neutralize' any FEM on
 * any context from doing any action, we can use ALL 'FEM programs'. So, set
 * the first available 'FEM program' to be the very first.
 * See dnx_field_actions_single_fem_single_context_defaults_set()
 * If we want to reserve the first FEM program for an invalid field group, we can increase the define by 1.
 */
#define FIRST_AVAILABLE_FEM_PROGRAM (DEFAULT_FEM_PROGRAM + 0)
/*
 * }
 */
/*
 * typedefs
 * {
 */
/**
 * \brief
 *   Logical representation of all fields (including key) related to a single
 *   condition on a single pair: 'FEM id' and 'FEM program'.
 *   Given this key, we can set/get the following:
 *     A single action (from FEM_*_MAP_TABLE)
 *     A flag Indicating whether action is valid
 *     A 'stock' of bits which could, potentially, be used
 *       for action-value building (Currently, not in use)
 * \see
 *   * IPPC_FEM_MAP_INDEX_TABLE
 *   * IPPC_FEM_*_MAP_TABLE
 */
typedef struct
{
    /**
     * Indicator for which 'FEM id' this structure is intended.
     */
    dnx_field_fem_id_t fem_id;
    /**
     * Indicator for which 'FEM program' this structure is intended.
     */
    dnx_field_fem_program_t fem_program;
    /**
     * Indicator for which 'condition' this structure is intended.
     */
    dnx_field_fem_condition_t fem_condition;
    /**
     * Info corresponding to action (map_index, map_data, 'valid' flag).
     */
    dnx_field_fem_condition_entry_t fem_condition_entry;
} dnx_field_fem_map_index_entry_t;
/**
 * \brief
 *   Logical representation of all fields related to a single action on a single FEM.
 *   Note that, depending on FEM id, the number of bits, composing action value,
 *   may be either 4 (FEMs 0/1) or 24 (FEMs 2/15).
 *   See IPPC_FEM_*_MAP_TABLE, and 'dnx_field_fem_bit_info_t'
 * \see
 *   * dnx_field_fem_bit_info_t
 *   * dnx_field_fem_bit_format_e
 *   * IPPC_FEM_*_MAP_TABLE
 */
typedef struct
{
    /**
     * Indicator for which 'FEM id' this structure is intended.
     * Actually, this is only used to ditinguish between one group (FEMs 0/1) and
     * the other (FEMs 2/15).
     */
    dnx_field_fem_id_t fem_id;
    /**
     * Indicator for which 'FEM map index' this structure is intended.
     * For each 'FEM condition', there are 4 indices, corresponding to 4 actions.
     */
    dnx_field_fem_map_index_t fem_map_index;
    /**
     * All information required for loading all bits per one 'action'.
     */
    dnx_field_fem_action_entry_t fem_action_entry;
} dnx_field_fem_action_info_t;
/**
 * \brief
 *   Logical representation of 'bit select' related to a single pair:
 *   'FEM id' and 'FEM program'.
 *   Given this key, we can set/get the following:
 *     A single 'fem_bit_select' (from IPPC_FEM_BIT_SELECT)
 * \see
 *   * IPPC_FEM_BIT_SELECT
 *   * IPPC_FEM_MAP_INDEX_TABLE
 */
typedef struct
{
    /**
     * Indicator for which 'FEM id' this structure is intended.
     */
    dnx_field_fem_id_t fem_id;
    /**
     * Indicator for which 'FEM program' this structure is intended.
     */
    dnx_field_fem_program_t fem_program;
    /**
     * Value of the MS bit of 4 bits, on 'key select', which are to be used as 'condition'.
     */
    dnx_field_fem_condition_ms_bit_t fem_condition_ms_bit;
} dnx_field_fem_condition_ms_bit_entry_t;
/**
 * \brief
 *   Logical representation of all fields related to a specific FEM on a
 *   specific context.
 *   Note that there are 4 such fields which correspond, then,to a pair:
 *   'context id' and 'FEM id'.
 * \see
 *   * dnx_field_pmf_fem_context_entry_t
 *   * IPPC_PMF_FEM_PROGRAM
 *   * FIELD_PMF_A_KEY_SELECT_N_FEM_PROG
 */
typedef struct
{
    /**
     * 'FEM program' (2 bits) on one entry on IPPC_PMF_FEM_PROGRAM.
     */
    dnx_field_fem_program_t fem_program;
    /**
     * A yes/no flag (1 bit) indicating whether the 'replace lsb' feature is enabled.
     */
    dbal_enum_value_field_field_fem_replace_lsb_flag_e replace_lsb_flag;
    /**
     * Identifier of TCAM-result from which the 16 LS bits are to be taken.
     * There are 8 TCAMs and, correspondingly, 8 results.
     */
    dbal_enum_value_field_field_fem_replace_lsb_select_e replace_lsb_select;
    /**
     * Identifier of the chunk of 32 bits (out of some result, or key) which will become
     * input to the FEM machine.
     */
    dbal_enum_value_field_field_pmf_a_fem_key_select_e fem_key_select;
} dnx_field_pmf_fem_program_t;
/**
 * \brief
 *   Logical representation of 'fem program and key select' related to a single pair:
 *   'FEM id' and 'context id' (which, together, are the key)
 *   Given this key, we can set/get the following, all on IPPC_PMF_FEM_PROGRAM):
 *     A single 'fem_key_select'
 *     A single 'FEM program'
 *     Two parameters required for the 'replace LSB' feature:
 *       Enable/disable this feature
 *       Identifier of the TCAM to take the 16 LS bits from
 * \see
 *   * IPPC_PMF_FEM_PROGRAM
 *   * FIELD_PMF_A_KEY_SELECT_N_FEM_PROG
 */
typedef struct
{
    /**
     * Indicator for which 'FEM id' this structure is intended.
     */
    dnx_field_fem_id_t fem_id;
    /**
     * Indicator for which 'context id' this structure is intended.
     */
    dnx_field_context_t context_id;
    /**
     * Data values, on IPPC_PMF_FEM_PROGRAM/FIELD_PMF_A_KEY_SELECT_N_FEM_PROG coresponding
     * to specified 'FEM id' on specified context.
     */
    dnx_field_pmf_fem_program_t pmf_fem_program;
} dnx_field_pmf_fem_context_entry_t;
/*
 * }
 */
/*
 * Procedures related to SW STATE for 'FEM machine'
 * {
 */
/**
 * \brief
 *   Given an IO source and a range of bits within it, get the corresponding FEM_KEY_SELECT
 *   value that identifies the 32 bits 'FEM_DATA' that contain this range.
 *   This procedure is specifically for 'dnx_field_group_fems_context_attach()'.
 *   If no such 'FEM_DATA' chunk is found, an indication is returned to the caller
 *   who may decide whether this is a fatal error or not.
 *   Note that this procedure is for FEM only.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] actions_block -
 *   Variable of type dbal_enum_value_field_field_io_e. Identifier of the 'IO' within which
 *   to search for specified range.
 * \param [in] action_size -
 *   Number of bits, within the range of bits, required for placing the action. This is the
 *   group of bits for which the 'shift' output is constructed (*required_shift_p).
 *   Note that for TCAM and Direct extraction, this procedure is called with
 *   a fixed 'action_size' of 32 bits (dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_key_select)
 * \param [in] action_lsb -
 *   The lsb (within the payload, starting at '0') of the group of 'action_size' bits.
 *   This is the group of bits for which the 'shift' output is constructed (*required_shift_p).
 *   Note that for TCAM and Direct extraction, this procedure is called with an 'action_lsb'
 *   which is a multiple of the resolution of 'fem key select'
 *   (dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->fem_key_select_resolution_in_bits) and, as a result,
 *   the calculated 'shift' will always be '0'.
 * \param [out] action_input_select_p -
 *   Pointer to unsigned int. Type of unsigned int is dbal_enum_value_field_field_pmf_a_fem_key_select_e.
 *   This procedure loads pointed memory by the identifier of the FEM_KEY_SELECT which carries this
 *   range of bits.
 * \param [out] required_shift_p -
 *   Pointer to uint. This procedure loads pointed memory by the the number of shifts required on the
 *   FEM_KEY_SELECT to get the action bits to the lsb location. See 'action_lsb' above.
 * \param [out] found_p -
 *   Pointer to int. This procedure loads pointed memory by a boolean: If matching KEY_SELECT
 *   has, indeed, been found then a non-zero value is returned. Otherwise, zero is returned.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   * Note that FEM usage implies IPMF1/2.
 * \see
 *   * dnx_field_group_fems_context_attach
 */
shr_error_e dnx_field_actions_fem_key_select_get(
    int unit,
    dbal_enum_value_field_field_io_e actions_block,
    unsigned int action_size,
    unsigned int action_lsb,
    dbal_enum_value_field_field_pmf_a_fem_key_select_e * action_input_select_p,
    unsigned int *required_shift_p,
    int *found_p);
/**
 * \brief
 *   Initialize SW STATE for the one 'FEM id' and one 'FEM program'
 * \param [in] unit     - Device ID
 * \param [in] fem_id -
 *    Indicator for which 'FEM id' this operation is intended.
 * \param [in] fem_program -
 *    Indicator for which 'FEM program' this SWSTATE setup is intended.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * See file field_actions.xml where all definitions, related to \n
 *     SW STATE of FEM, are stored. Root is 'dnx_field_action_sw_db'
 * \see
 *   dnx_field_fem_state_t
 */
shr_error_e dnx_field_single_fem_prg_sw_state_init(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program);

/*
 * Utility procedures to be used by, say, diag_dnx_field and ctest_dnx_field
 * {
 */
/**
 * \brief
 *  This function will indicate fem allocation state per field group: Is there any
 *  occupied fem as per specified filters below then indicate that.
 *  Filters include involved Field Groups and 'action's.
 *  Note that filters are used with AND logic: ALL filters must match
 *  to result in a final match (I.e., indication of 'occupied').
 * \param [in] unit  -
 *   HW device ID
 * \param [in] context_id -
 *    Indicator for which 'context' this function checks against. used for FEMs that are invalidated per context.
 *    If set to 'DNX_FIELD_CONTEXT_ID_INVALID', this filter is ignored.
 * \param [in] fg_id -
 *   Identifier of a field group to search for on the list of active FEMs. If
 *   set to 'DNX_FIELD_GROUP_INVALID' then this filter is ignored ('fg_id' is
 *   'do not care').
 * \param [in] second_fg_id -
 *   Identifier of a 'second field group' to search for on the list of active FEMs. If
 *   set to 'DNX_FIELD_GROUP_INVALID' then this filter is ignored ('second_fg_id' is
 *   'do not care').
 * \param [in] active_actions -
 *    Bit map of 4 bits. Each bit stands for a 'fem_action_entry'. If ALL of the
 *    'action's, specified this way, are found 'set' then search is successful.
 *    If set to a value of DNX_FIELD_IGNORE_ALL_ACTIONS then this filter is ignored
 *    ('active_actions' is 'do not care').
 *    Bits beyond the the 4 relevant bits (dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs)
 *    are ignored.
 * \param [out] none_occupied_p            -
 *   int pointer. This procedure loads pointed memory by a non-zero value
 *   if no key is occupied for this stage/context pair.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * Output of this procedure is based on FEMs occupation as stored on SW state.
 * \see
 *   * See dnx_field_key_is_any_key_occupied()
 */
shr_error_e dnx_field_fem_is_any_fem_occupied_on_fg(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_group_t second_fg_id,
    uint8 active_actions,
    int *none_occupied_p);
/*
 * }
 */

/**
 * \brief
 *   Initialize SW STATE for the whole 'FEM' module (all 'FEM id's).
 * \param [in] unit     - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * See file field_actions.xml where all definitions, related to \n
 *     SW STATE of FEM, are stored. Root is 'dnx_field_action_sw_db'
 * \see
 *   dnx_field_fem_state_t
 */
shr_error_e dnx_field_all_fems_sw_state_init(
    int unit);
/*
 * }
 */
/**
 * \brief
 *   Load initial values into structure of type dnx_field_fem_condition_entry_t.
 *   Values are such that no action will be carried out.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in,out] struct_p -
 *    As input: \n
 *      This is a pointer to a structure to be filled.
 *    As output: \n
 *      This is a pointer to the structure which has been filled by initial values.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * None
 * \see
 *   * dnx_field_fem_condition_entry_t
 *   * dnx_field_fem_condition_entry_t_array_init
 */
shr_error_e dnx_field_fem_condition_entry_t_init(
    int unit,
    dnx_field_fem_condition_entry_t * struct_p);
/**
 * \brief
 *   Load initial values into array of structures of type dnx_field_fem_condition_entry_t.
 *   Size of array is taken from DATA and is as specified by HW.
 *   See DBAL table FIELD_PMF_A_FEM_MAP_INDEX
 *   Values are such that no action will be carried out.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in,out] fem_condition_entry -
 *    As input: \n
 *      This is a pointer to the array of structures to be filled.
 *    As output: \n
 *      This is a pointer to the array of structures which has been filled by initial values.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * None
 * \see
 *   * dnx_field_fem_condition_entry_t
 */
shr_error_e dnx_field_fem_condition_entry_t_array_init(
    int unit,
    dnx_field_fem_condition_entry_t fem_condition_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION]);
/**
 * \brief
 *   Indicate whether 'fg_id' is marked as one of the owners of 'fem_id'
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] fg_id -
 *   Identifier of a field group to search for on the list of owners of 'fem_id'.
 * \param [out] fem_program_p -
 *   This procedure loads pointed memory by the 'fem_program' which correponds
 *   to specified 'fg_id' on specified 'fem_id'. If 'fg_id' is not owner
 *   of 'fem_id', a value of DNX_FIELD_FEM_PROGRAM_INVALID is loaded.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * If 'fg_id' does not own 'fem_id', see 'fem_program_p' above. 
 *    * If 'fg_id' is not allocated, an error is ejected.
 *    * If 'fg_id' is not on IPMF2 or IPMF1, an error is ejected.
 * \see
 *   * dnx_field_fem_action_add()
 */
shr_error_e dnx_field_fem_is_fg_owner(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_group_t fg_id,
    dnx_field_fem_program_t * fem_program_p);

/**
 * \brief
 *   Get next pair ('fem_id','fem_program') which are assigned to specified 'fg_id'.
 *   In other words: Search all FEMs, staring from specified one, to find whether
 *   'fg_id' is there as an 'owner'. An 'fg_id' is considered 'owner' if any
 *   FEM has been 'added' to it.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fg_id -
 *   Identifier of a field group to search for on the list of owners of 'fem_id's.
 * \param [in,out] fem_id_p -
 *   \b As \b input - \n
 *     Indicator of the first 'fem_id' to start searching at (until the last 'fem_id').
 *     Specified 'fem_id' is NOT included in the search.
 *     If set to DNX_FIELD_FEM_ID_INVALID then start searching from first 'fem_id'.
 *   \b As \b output - \n
 *     This procedure loads pointed memory by an indicator for which 'fem_id' was
 *     found to have been 'added' to 'fg_id'.
 *     If set to DNX_FIELD_FEM_ID_INVALID then no match has been found.
 * \param [out] fem_program_p -
 *   This procedure loads pointed memory by the 'fem_program' which correponds
 *   to specified 'fg_id' on specified '*fem_id_p'. Only meaningful if '*fem_id_p'
 *   is NOT DNX_FIELD_FEM_ID_INVALID.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * If 'fg_id' does not own any 'fem_id', then caller would load '*fem_id_p'
 *      by DNX_FIELD_FEM_ID_INVALID as input and would get the same as procedure output.. 
 *    * If 'fg_id' is not allocated, an error is ejected.
 *    * If 'fg_id' is not on IPMF2 or IPMF1, an error is ejected.
 * \see
 *   * dnx_field_fem_action_add()
 *   * dnx_field_fem_is_fg_owner()
 */
shr_error_e dnx_field_fem_get_next_fg_owner(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_fem_id_t * fem_id_p,
    dnx_field_fem_program_t * fem_program_p);

/**
 * \brief
 *   Indicate whether 'second_fg_id' is marked as being on 'fem_id' (on one of
 *   its 'fem_program's)
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] second_fg_id -
 *   Identifier of a field group to search for on the list of 'second'
 *   field groups of 'fem_id'.
 * \param [out] fem_program_p -
 *   This procedure loads pointed memory by the 'fem_program' which correponds
 *   to specified 'second_fg_id' on specified 'fem_id'. If 'second_fg_id' is not
 *   a 'second' field group of 'fem_id', a value of DNX_FIELD_FEM_PROGRAM_INVALID
 *   is loaded.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * If 'second_fg_id' is not 'second' field group on 'fem_id', see 'fem_program_p' above. 
 *    * If 'second_fg_id' is not allocated, an error is ejected.
 *    * If 'second_fg_id' is not on IPMF2 or IPMF1, an error is ejected.
 * \see
 *   * dnx_field_fem_action_add()
 */
shr_error_e dnx_field_fem_is_fg_secondary(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_group_t second_fg_id,
    dnx_field_fem_program_t * fem_program_p);

/**
 * \brief
 *   Indicate whether 'second_fg_id' is marked as being on ANY 'fem_id'
 *   (on one of its 'fem_program's)
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] second_fg_id -
 *   Identifier of a field group to search for on the list of 'second'
 *   field groups of all 'fem_id's.
 * \param [out] fem_id_p -
 *   This procedure loads pointed memory by a 'fem_id' which carries 'second_fg_id'
 *   as an active secondary field group. If 'second_fg_id' is not a 'second' field
 *   group of any 'fem_id', a value of DNX_FIELD_FEM_ID_INVALID is loaded.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * If 'second_fg_id' is not 'second' field group on any 'fem_id', see 'fem_id_p' above. 
 *    * If 'second_fg_id' is not allocated, an error is ejected.
 *    * If 'second_fg_id' is not on IPMF2 or IPMF1, it is indicated with DNX_FIELD_FEM_ID_INVALID.
 * \see
 *   * dnx_field_fem_action_add()
 */
shr_error_e dnx_field_fem_is_fg_secondary_on_any(
    int unit,
    dnx_field_group_t second_fg_id,
    dnx_field_fem_id_t * fem_id_p);

/**
 * \brief
 *   Verify that the number of valid bits, specified for input 'fem_id',
 *   is correct: 4 bits for 'FEM 0/1' and 24 for 'FEM 2/23'
 *   Also verify that 'adder' is only specified for 'FEM 2/23'
 *   If the number of bits (or adder) is not correct, an error is returned.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] dnx_encoded_fem_action -
 *   Encoded value of action corresponding to one 'action type'
 *   on 'fem_action_entry_p[]' below. We need to extract the stage from this action
 *   and to get action size for verification.
 * \param [in] fem_action_entry_p -
 *    Pointer to structure containing all info corresponding to one 'action'
 *    (= all 4/24 bit descriptors).
 *    See, for example, DBAL tables: FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_*_MAP
 *    Note that, for 'FEM id's 0/1, only 4 bits may be specified. In that case,
 *    the per-bit element 'fem_bit_format' on 'dnx_field_fem_action_entry_t'
 *    should be marked DNX_FIELD_FEM_BIT_FORMAT_INVALID for bits 4 to 23.  Otherwise,
 *    an error will be ejected.
 *    and the value should be 'zero'. Also, 'adder' must be 0.
 *    For 'FEM id's 2/23, all 24 bits must be specified. In that case, the per-bit element
 *    'fem_bit_format' on 'dnx_field_fem_action_entry_t' should NOT be marked
 *    DNX_FIELD_FEM_BIT_FORMAT_INVALID.  Also, all 'adder' bits above the first
 *    24 () must be zero. Otherwise, an error will be ejected.
 *   
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_field_fem_action_add()
 */
shr_error_e dnx_field_fem_num_action_bits_verify(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_action_t dnx_encoded_fem_action,
    dnx_field_fem_action_entry_t * fem_action_entry_p);
/**
 * \brief
 *   Load initial values into structure of type dnx_field_pmf_fem_context_entry_t.
 *   Values are such that no action will be carried out.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in,out] struct_p -
 *    As input: \n
 *      This is a pointer to a structure to be filled.
 *    As output: \n
 *      This is a pointer to the structure which has been filled by initial values.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * None
 * \see
 *   * dnx_field_pmf_fem_context_entry_t
 *   * dnx_field_pmf_fem_program_t
 */
shr_error_e dnx_field_pmf_fem_context_entry_t_init(
    int unit,
    dnx_field_pmf_fem_context_entry_t * struct_p);
/**
 * \brief
 *   Load initial values into structure of type dnx_field_fem_action_entry_t.
 *   Values are such that no action will be carried out.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in,out] struct_p -
 *    As input: \n
 *      This is a pointer to a structure to be filled.
 *    As output: \n
 *      This is a pointer to the structure which has been filled by initial values.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * None
 * \see
 *   * dnx_field_fem_action_entry_t
 */
shr_error_e dnx_field_fem_action_entry_t_init(
    int unit,
    dnx_field_fem_action_entry_t * struct_p);

/**
 * \brief
 *   Indicate whether the context uses a specific FEM ID
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] context_id -
 *    Indicator for which 'context' this operation is intended.
 * \param [in] fg_id -
 *    Optional indicator for testing if the FEM ID is allocated for the context to the field group.
 *    if equals DNX_FIELD_GROUP_TYPE_INVALID, will verify for all field groups.
 * \param [out] using_fem_p -
 *    This procedure loads pointed memory by a non-zero value if the context used a valid input type for the fem_id.
 *    Otherwise, 'zero' is loaded.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * None
 * \see
 *   * dnx_field_group_fems_context_detach()
 */
shr_error_e dnx_field_fem_is_context_using(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    unsigned int *using_fem_p);

/**
 * \brief
 *   Indicate whether input 'fem id' is active on input 'fem context', in HW.
 *   If so, entry is considered 'valid'.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] context_id -
 *    Indicator for which 'context' this operation is intended.
 * \param [out] is_valid_p -
 *    This procedure loads pointed memory by a non-zero value if specified
 *    'fem context' entry is valid on specified 'fem_id'.
 *    Otherwise, 'zero' is loaded.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * None
 * \see
 *   * dnx_field_group_fems_context_detach()
 */
shr_error_e dnx_field_fem_is_context_valid(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_context_t context_id,
    unsigned int *is_valid_p);

/**
 * \brief
 *   Verify that input FEM context, in HW, is valid and has the specified 'fem_program'
 *   on the specified 'fem_id'.
 *   If so, entry is considered 'consistent'.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] fem_program -
 *    Indicator for which 'FEM program' we are seraching for.
 * \param [in] context_id -
 *    Indicator for which 'context' this operation is intended.
 * \param [out] consistent_p -
 *    This procedure loads pointed memory by a non-zero value if specified
 *    'fem context' entry is valid and carries this 'fem_program' (on specified
 *    'fem_id').
 *    Otherwise, 'zero' is loaded.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * None
 * \see
 *   * dnx_field_fem_action_remove()
 */
shr_error_e dnx_field_fem_is_context_consistent(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_context_t context_id,
    unsigned int *consistent_p);
/**
 * \brief
 *   Verify that input FEM context, in HW, is valid and has the specified 'fem_program'
 *   on the specified 'fem_id' together with a valid 'replace_lsb_select' and
 *   'replace_lsb_flag' which indicates 'active'.
 *   If so, entry is considered 'valid replace'.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] fem_program -
 *    Indicator for which 'FEM program' we are seraching for.
 * \param [in] context_id -
 *    Indicator for which 'context' this operation is intended.
 * \param [out] valid_replace_p -
 *    This procedure loads pointed memory by a non-zero value if specified
 *    'fem context' entry is valid and carries this 'fem_program' (on specified
 *    'fem_id') plus valid 'replace' info.
 *    Otherwise, 'zero' is loaded.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * None
 * \see
 *   * dnx_field_fem_action_remove()
 */
shr_error_e dnx_field_fem_is_context_valid_replace(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_context_t context_id,
    unsigned int *valid_replace_p);

/**
 * \brief
 *   Search all FEM contexts in HW. Look for an active context which has the specified
 *   'fem_program' on the specified 'fem_id' with valid 'replace' info. (This indicates
 *   that ('fem_id','fem_program') pair is active and is related to some 'second_fg_id').
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] fem_program -
 *    Indicator for which 'FEM program' we are searching for.
 * \param [out] context_id_p -
 *    This procedure loads pointed memory by a valid 'context_id' if an active context
 *    carrying this 'fem_program' (on specified 'fem_id') and valid 'replace' info
 *    is found.
 *    Otherwise, the value of DNX_FIELD_CONTEXT_ID_INVALID is loaded.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * None
 * \see
 *   * dnx_field_fem_action_remove()
 *   * dnx_field_fem_action_add()
 */
shr_error_e dnx_field_fem_is_any_context_with_replace(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_context_t * context_id_p);

/**
 * \brief
 *   Search all FEM contexts in HW. Look for an active context which has the specified
 *   'fem_program' on the specified 'fem_id'. (This indicates that ('fem_id','fem_program')
 *   pair is active).
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] fem_program -
 *    Indicator for which 'FEM program' we are searching for.
 * \param [out] context_id_p -
 *    This procedure loads pointed memory by a valid 'context_id' if an active context
 *    carrying this 'fem_program' (on specified 'fem_id') is found.
 *    Otherwise, the value of DNX_FIELD_CONTEXT_ID_INVALID is loaded.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * None
 * \see
 *   * dnx_field_fem_action_remove()
 *   * dnx_field_fem_action_add()
 */
shr_error_e dnx_field_fem_is_any_context_with_program(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_context_t * context_id_p);

/**
 * \brief
 *   Remove a specific FEM machine from specified Field Group. The Field Group,
 *   itself, remains active.
 *   All contexts attached to this Field Group are supposed to have been 'detached'.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] fg_id -
 *   Identifier of a field group from which FEM machine is to be removed.
 *   Note that this FG may be of a few types: DirectExtraction, TCAM, EXEM, MDB.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * 'FEM program', which was selected on dnx_field_fem_action_add(),
 *      is 'released' by this procedure.
 *    * If 'fg_id' or 'fem_id' is not within legal range, an error is ejected. 
 *    * If 'fg_id' does not own 'fem_id', an error is ejected. 
 *    * If 'fg_id' is not on IPMF1/IPMF2, an error is ejected. 
 * \see
 *   * dnx_field_fem_action_add()
 */
shr_error_e dnx_field_fem_action_remove(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_group_t fg_id);
/**
 * \brief
 *   Add FEM machine as a resource to field group.
 *   Added FEM will trigger a specified action depending on specified conditions.
 *   Input for FEM is specified 32 bits known as 'key select'. It is specified when
 *   context is attached to this field group.
 *   NOTE: This procedure MUST be called before any context is attached to this field group!
 *   See remarks and references below.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *    Indicator for which 'FEM id' this operation is intended.
 * \param [in] fg_id -
 *   Identifier of a field group to which FEM machine is to be added.
 *   Note that this FG may be of a few types: DirectExtraction, TCAM, EXEM, MDB. The type
 *   of this FG will be used to determine the IO of the FEM. See 'dbal_enum_value_field_field_io_e'.
 *   The IO is required for choosing the proper 'fem key select'.
 *   See 'dbal_enum_value_field_field_pmf_a_fem_key_select_e'
 * \param [in] input_offset -
 *   Indicates location, starting from LS bit, on the key template of indicated 'field group'.
 *   This is where the meanigful data (in most cases, of 32-bits size), known as 'fem key
 *   select', resides. See 'fg_id' above.
 *   Note that for FG types such as EXEM, some 32-bits 'chunks' may contain 'padding' bits that
 *   are not meaningful.
 *   For example, the 'chunk' marked DBAL_ENUM_FVAL_FIELD_PMF_A_FEM_KEY_SELECT_SEXEM_27_0_4_B0
 *   has 4 padding bits.
 *   For the same IO (IO stands for, say, 'key I'), all FEMs should have this input differ by
 *   a multiple of '16'. (For exaple, one FEM could have '7' and the other could have 23 (7 + 16).
 * \param [in] input_size -
 *   Indicates the number of meanigful bits (usually 32), that are required to be on the 'chunk'
 *   ('fem key select'). See 'fg_id' above.
 *   In most cases, this input will be be '32'. However,there are exeptions such as EXEM.
 *   See explanation on 'input_offset' above. For that example, one would set 'input_size'
 *   to '28'
 * \param [in] second_fg_id -
 *   Identifier of a TCAM field group from whose result, the LS 16 bits are to be taken.
 *   A value of DNX_FIELD_GROUP_INVALID indicates 'no such field group'.
 *   If a valid value is specified, these 16 bits will replace the LS 16 bits on the
 *   32-bits 'key select' that is specified by 'input_offset' above. 
 *   This procedure verifies this is a TCAM field group.
 *   Note that, if 'second_fg_id' is not DNX_FIELD_GROUP_INVALID, then when attaching
 *   context to 'fg_id', that same context (or, in case of cascading, the 'father' context)
 *   must have been attached to 'second_fg_id'. Otherwise, an error is injected.
 * \param [in] fem_condition_ms_bit -
 *   Five bits offset, with the 32-bits 'fem key select'. This value indicates the location
 *   of the MS bit of the group of 4 bits, on 'key select, to extract condition from.
 *   See DBAL table: FIELD_PMF_A_FEM_CONDITION_MS_BIT
 * \param [in] fem_condition_entry -
 *   Array of '16' elements (size taken from DATA). Each element contains all info corresponding
 *   to one 'condition'. Among these parameters is an indication on which 'action' (one of the
 *   4 available) is to be activated when this condition is hit. The available 4 'action's are
 *   detailed on 'fem_action_entry' below. See DBAL table: FIELD_PMF_A_FEM_MAP_INDEX
 *   Note that only 'action' indices, that are referred to here, are referenced on 'fem_action_entry'.
 *   For example, if, on all 16 conditions, only indices '0' and '1' are specified then only
 *   fem_action_entry[0] and fem_action_entry[1] will be looked up. The othres will be
 *   ignored.
 * \param [in] fem_encoded_actions -
 *    Array of '4' elements (size taken from DATA). Each element contains encoded value of
 *    action corresponding to one 'action type' on 'fem_action_entry[]' below.
 * \param [in] fem_action_entry -
 *    Array of '4' elements (size taken from DATA). Each element contains all info corresponding
 *    to one 'action' (= all 4/24 bit descriptors).
 *    See, for example, DBAL tables: FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_*_MAP
 *    Note that, for 'FEM id's 0/1, only 4 bits may be specified. In that case, the per-bit element
 *    'fem_bit_format' on 'dnx_field_fem_action_entry_t' should be marked DNX_FIELD_FEM_BIT_FORMAT_INVALID
 *    for bits 4 to 23. Otherwise, an error will be ejected.
 *    Note that the 'fem_action' element is not an input parameter. It is not encoded! This is
 *    'action_type' as written to HW. The input encoded actions are on 'fem_encoded_actions' above.
 *    Note the 'fem_adder' element -
 *    Element contains a 24-bits value to add to action value after all bit-extractions from 'key select'.
 *    See DBAL table: FIELD_PMF_A_FEM_ADDER_FEMS_2_15_MAP
 *    Note that, for 'FEM id's 0/1, this input must be set to 0.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * 'FEM program' is selected automatically by this procedure.
 * \see
 *   * dnx_field_fem_action_remove
 *   * dnx_field_fem_condition_ms_bit_entry_t
 *   * dnx_field_fem_condition_entry_t
 *   * dnx_field_fem_action_entry_t
 */
shr_error_e dnx_field_fem_action_add(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_group_t fg_id,
    uint8 input_offset,
    uint8 input_size,
    dnx_field_group_t second_fg_id,
    dnx_field_fem_condition_ms_bit_t fem_condition_ms_bit,
    dnx_field_fem_condition_entry_t fem_condition_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    dnx_field_action_t fem_encoded_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX],
    dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX]);
/**
 * \brief
 *   Update hardware: For given 'FEM id' and 'FEM program', update the corresponding
 *   'context independent' section of FEM hardware tables with defult values.
 *   See remarks and references below.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *    Indicator for which 'FEM id' this HW setup is intended.
 *    See, for example, DBAL table: FIELD_PMF_A_FEM_CONDITION_MS_BIT
 * \param [in] fem_program -
 *    Indicator for which 'FEM program' this HW setup is intended.
 *    See, for example, DBAL table: FIELD_PMF_A_FEM_CONDITION_MS_BIT
 * \param [in] ignore_actions -
 *    Bit map of 4 bits. If a bit is non-zero then corrsponding 'fem_action_entry' (incl. 'fem_adder')
 *    is ignored (i.e., They are NOT loaded by default values).
 *    For example, if BIT(0) is '1' then 'fem_action_entry[0]'
 *    is ignored. See dnx_field_actions_fem_set().
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * Default values are selected so that no action will be carried out when this pair
 *      (FEM id, FEM program) is activated by any context.
 * \see
 *   * dnx_field_fem_condition_ms_bit_entry_t
 *   * dnx_field_fem_condition_entry_t
 *   * dnx_field_fem_action_entry_t
 */
shr_error_e dnx_field_actions_fem_defaults_set(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    uint8 ignore_actions);
/**
 * \brief
 *   Update hardware: For given 'FEM id','FEM program', 'conditions' and 'actions', update
 *   the corresponding 'context independent' section of FEM hardware tables.
 *   See remarks and references below.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *    Indicator for which 'FEM id' this HW setup is intended.
 *    See, for example, DBAL table: FIELD_PMF_A_FEM_CONDITION_MS_BIT
 * \param [in] fem_program -
 *    Indicator for which 'FEM program' this HW setup is intended.
 *    See, for example, DBAL table: FIELD_PMF_A_FEM_CONDITION_MS_BIT
 * \param [in] fem_condition_ms_bit -
 *    Value of 'bit select' (group of 4 bits, on 'key select, to extract condition from).
 *    See DBAL table: FIELD_PMF_A_FEM_CONDITION_MS_BIT
 * \param [in] fem_condition_entry -
 *    Array of '16' elements (size taken from DATA). Each element contains all info corresponding
 *    to one 'condition'. See DBAL table: FIELD_PMF_A_FEM_MAP_INDEX
 * \param [in] ignore_actions -
 *    Bit map of 4 bits. If a bit is non-zero then corrsponding 'fem_action_entry' (incl. 'fem_adder')
 *    is ignored. For example, if BIT(0) is '1' then 'fem_action_entry[0]'
 *    is ignored. This allows for actions of the same FEM to be used, via a few 'FEM programs',
 *    by more than one FG or, even, by the same FG (but for a different set of conditions).
 *    Note that if the caller tries to overwrite an action/adder which is NOT marked as 'invalid'
 *    then this procedure will eject an error!
 * \param [in] fem_action_entry -
 *    Array of '4' elements (size taken from DATA). Each element contains all info corresponding
 *    to one 'action' (= all 4/24 bit descriptors). See, for example, DBAL tables: FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_*_MAP
 *    Note the 'fem_adder' element -
 *    Element contains a 24-bits value to add to
 *    action value after all bit-extractions from 'key select'.
 *    See DBAL table: FIELD_PMF_A_FEM_ADDER_FEMS_2_15_MAP
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * Note that info on 'dnx_field_fem_condition_ms_bit_entry_p' corresponds to sixteen 'FEM condition's
 * \see
 *   * dnx_field_fem_condition_ms_bit_entry_t
 *   * dnx_field_fem_condition_entry_t
 *   * dnx_field_fem_action_entry_t
 */
shr_error_e dnx_field_actions_fem_set(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_fem_condition_ms_bit_t fem_condition_ms_bit,
    dnx_field_fem_condition_entry_t fem_condition_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    uint8 ignore_actions,
    dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX]);
/**
 * \brief
 *   Get info from hardware: For given 'FEM id','FEM program',
 *   get the corresponding 'context independent' section of FEM hardware
 *   tables ('condition_ms_bit', 'conditions' and 'actions').
 *   See remarks and references below.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *    Indicator for which 'FEM id' this HW setup is intended.
 *    See, for example, DBAL table: FIELD_PMF_A_FEM_CONDITION_MS_BIT
 * \param [in] fem_program -
 *    Indicator for which 'FEM program' this HW setup is intended.
 *    See, for example, DBAL table: FIELD_PMF_A_FEM_CONDITION_MS_BIT
 * \param [out] fem_condition_ms_bit_p -
 *    Pointer to 'dnx_field_fem_condition_ms_bit_t'. This procedure loads pointed memory by the
 *    value of 'bit select' (group of 4 bits, on 'key select, to extract condition from).
 *    See DBAL table: FIELD_PMF_A_FEM_CONDITION_MS_BIT
 * \param [out] fem_condition_entry -
 *    Array of 16 elements of type 'dnx_field_fem_condition_entry_t'. This procedure loads
 *    this array. Each element is loaded by all info corresponding to one 'condition'.
 *    See DBAL table: FIELD_PMF_A_FEM_MAP_INDEX
 * \param [out] fem_action_entry -
 *    Array of '4' elements of type 'dnx_field_fem_action_entry_t'. This procedure loads
 *    each element by all info corresponding to one 'action' (= all 4/24 bit descriptors).
 *    See, for example, DBAL tables: FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_*_MAP
 *    Note the 'fem_adder' element -
 *    Element of type 'dnx_field_fem_adder_t'. This procedure loads these elements
 *    by a 24-bits value (intended to be added to action value after all bit-extractions
 *    from 'key select').
 *    See DBAL table: FIELD_PMF_A_FEM_ADDER_FEMS_2_15_MAP
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * Note that info on 'dnx_field_fem_condition_ms_bit_entry_p' corresponds to sixteen 'FEM condition's
 * \see
 *   * dnx_field_fem_condition_ms_bit_entry_t
 *   * dnx_field_fem_condition_entry_t
 *   * dnx_field_fem_action_entry_t
 */
shr_error_e dnx_field_actions_fem_get(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    dnx_field_fem_condition_ms_bit_t * fem_condition_ms_bit_p,
    dnx_field_fem_condition_entry_t fem_condition_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX]);

/**
 * \brief
 *   Update hardware: For given 'FEM id','context id', load default values on
 *   specified FEM (contained on that single 'context').
 *   Updated HW table is: IPPC_PMF_FEM_PROGRAM
 *   Updated DBAL table is: FIELD_PMF_A_KEY_SELECT_N_FEM_PROG
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] context_id -
 *    The single 'context' to load default values on (for all FEMs).
 * \param [in] fem_id -
 *    Identifier of FEM to be updated on specified 'context'
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * None
 * \see
 *   * dnx_field_pmf_fem_context_entry_t
 */
shr_error_e dnx_field_actions_single_fem_single_context_defaults_set(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_fem_id_t fem_id);
/**
 * \brief
 *   Update hardware: For given 'context id', load default values on all
 *   FEMs contained on a single 'context'.
 *   Updated HW table is: IPPC_PMF_FEM_PROGRAM
 *   Updated DBAL table is: FIELD_PMF_A_KEY_SELECT_N_FEM_PROG
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] context_id -
 *    The single 'context' to load default values on (for all FEMs).
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * None
 * \see
 *   * dnx_field_pmf_fem_context_entry_t
 */
shr_error_e dnx_field_actions_all_fem_single_context_defaults_set(
    int unit,
    dnx_field_context_t context_id);
/**
 * \brief
 *   Update hardware: Load default values on all 'context's (and their
 *   corresponding FEMs).
 *   Updated HW table is: IPPC_PMF_FEM_PROGRAM
 *   Updated DBAL table is: FIELD_PMF_A_KEY_SELECT_N_FEM_PROG
 * \param [in] unit -
 *   Identifier of HW platform.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * None
 * \see
 *   * dnx_field_pmf_fem_context_entry_t
 */
shr_error_e dnx_field_actions_all_fem_all_context_defaults_set(
    int unit);
/**
 * \brief
 *   Update hardware: For given 'FEM id','context id' (contained within descriptor), update
 *   the corresponding 'FEM program', 'FEM key select' and 'replace lsb' parameters.
 *   Updated HW table is: IPPC_PMF_FEM_PROGRAM
 *   Updated DBAL table is: FIELD_PMF_A_KEY_SELECT_N_FEM_PROG
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] dnx_field_pmf_fem_program_entry_p -
 *    Structure containing all info regarding one FEM on one 'context'.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * None
 * \see
 *   * dnx_field_pmf_fem_context_entry_t
 */
shr_error_e dnx_field_actions_fem_context_hw_set(
    int unit,
    dnx_field_pmf_fem_context_entry_t * dnx_field_pmf_fem_program_entry_p);
/**
 * \brief
 *   Get info from hardware: For given 'FEM id','context id' (contained within descriptor),
 *   retrieve the corresponding 'FEM program', 'FEM key select' and 'replace lsb' parameters.
 *   Accessed HW table is: IPPC_PMF_FEM_PROGRAM
 *   Accessed DBAL table is: FIELD_PMF_A_KEY_SELECT_N_FEM_PROG
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in,out] dnx_field_pmf_fem_program_entry_p -
 *    Structure containing all info regarding one FEM on one 'context'.
 *    As input:
 *      fem_id -
 *        Identifier of FEM to retrieve info for.
 *      context_id -
 *        Identifier of 'FEM programcontext' to retrieve info for.
 *    As output:
 *      pmf_fem_program -
 *        Structure of type dnx_field_pmf_fem_program_t containing the retrieved info.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * None
 * \see
 *   * dnx_field_pmf_fem_context_entry_t
 */
shr_error_e dnx_field_actions_fem_context_hw_get(
    int unit,
    dnx_field_pmf_fem_context_entry_t * dnx_field_pmf_fem_program_entry_p);
/**
 * \brief
 *   Update hardware: For given 'FEM id','FEM program' (contained within descriptor), update
 *   the corresponding 'bit select' which indicates which 4 bits to select as 'condition'.
 *   Note that 'bit select' selects one of sixteen actions available per one ('FEM id','FEM program','condition')
 *   Updated HW table is: IPPC_FEM_BIT_SELECT
 *   Updated DBAL table is: FIELD_PMF_A_FEM_CONDITION_MS_BIT
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] dnx_field_fem_condition_ms_bit_entry_p -
 *    Structure containing all info regarding one 'bit select' entry.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * Note that info on 'dnx_field_fem_condition_ms_bit_entry_p' corresponds to sixteen 'FEM condition's
 * \see
 *   * dnx_field_fem_condition_ms_bit_entry_t
 */
shr_error_e dnx_field_actions_fem_condition_ms_bit_hw_set(
    int unit,
    dnx_field_fem_condition_ms_bit_entry_t * dnx_field_fem_condition_ms_bit_entry_p);
/**
 * \brief
 *   Get info from hardware: For given 'FEM id','FEM program' (contained within descriptor), get
 *   the corresponding 'bit select' which indicates which 4 bits to select as 'condition'.
 *   Note that 'bit select' selects one of sixteen actions available per one ('FEM id','FEM program','condition')
 *   Info retrieved from HW table: IPPC_FEM_BIT_SELECT
 *   Info retrieved from DBAL table: FIELD_PMF_A_FEM_CONDITION_MS_BIT
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in,out] dnx_field_fem_condition_ms_bit_entry_p -
 *    Structure containing all info regarding one 'bit select' entry.
 *    As input:
 *      fem_id -
 *        Identifier of FEM to retrieve info for.
 *      fem_program -
 *        Identifier of 'FEM program' to retrieve info for.
 *    As output:
 *      fem_condition_ms_bit -
 *        The retrieved info: Index of the MS bit of the four bits on 'key select' which are
 *        to contain the 'condition'.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * Note that info on 'dnx_field_fem_condition_ms_bit_entry_p' corresponds to sixteen 'FEM condition's
 * \see
 *   * dnx_field_fem_condition_ms_bit_entry_t
 */
shr_error_e dnx_field_actions_fem_condition_ms_bit_hw_get(
    int unit,
    dnx_field_fem_condition_ms_bit_entry_t * dnx_field_fem_condition_ms_bit_entry_p);
/**
 * \brief
 *   Update hardware: For given 'FEM id','FEM program' and 'condition' (contained within descriptor) update
 *   all info corresponding ('map index', 'map data' and 'action valid').
 *   Note that 'map index' selects one of four actions available per one ('FEM id','FEM program')
 *   Updated HW table is: IPPC_FEM_MAP_INDEX_TABLE
 *   Updated DBAL table is: FIELD_PMF_A_FEM_MAP_INDEX
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] dnx_field_fem_map_index_entry_p -
 *    Structure containing all info regarding one 'map index' entry.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * Note that info on 'dnx_field_fem_map_index_entry_p' corresponds to one 'FEM condition'
 * \see
 *   * dnx_field_fem_map_index_entry_t
 */
shr_error_e dnx_field_actions_fem_map_index_hw_set(
    int unit,
    dnx_field_fem_map_index_entry_t * dnx_field_fem_map_index_entry_p);
/**
 * \brief
 *   Get info from hardware: For given 'FEM id','FEM program' and 'condition' (contained within descriptor)
 *   get all info corresponding ('map index', 'map data' and 'action valid').
 *   Note that 'map index' selects one of four actions available per one ('FEM id','FEM program')
 *   Accessed HW table is: IPPC_FEM_MAP_INDEX_TABLE
 *   Accessed DBAL table is: FIELD_PMF_A_FEM_MAP_INDEX
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in,out] dnx_field_fem_map_index_entry_p -
 *    Structure to contain all info regarding one 'map index' entry.
 *    As input:
 *      fem_id -
 *        Identifier of FEM to retrieve info for.
 *      fem_program -
 *        Identifier of 'FEM program' to retrieve info for.
 *      fem_condition -
 *        Identifier of the 'condition' (one of 16) to retrieve info for.
 *    As output:
 *      fem_map_index -
 *        The index (one of 4) of the action to carry out
 *      fem_map_data -
 *        Four bits that may be used to construct the action
 *      fem_action_valid -
 *        Flag indicating whether all actions correcponding to this entry are valid or not.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * Note that info on 'dnx_field_fem_map_index_entry_p' corresponds to one 'FEM condition'
 * \see
 *   * dnx_field_fem_map_index_entry_t
 */
shr_error_e dnx_field_actions_fem_map_index_hw_get(
    int unit,
    dnx_field_fem_map_index_entry_t * dnx_field_fem_map_index_entry_p);
/**
 * \brief
 *   Update hardware: For given 'FEM id' and 'map index' (contained within descriptor) update
 *   all info corresponding to one 'FEM action'.
 *   Note that 'map index' selects one of four actions available per one 'FEM id'
 *   Updated HW tables are: IPPC_FEM_*_4B_MAP_TABLE, IPPC_FEM_*_24B_MAP_TABLE
 *   Updated DBAL tables are: FIELD_PMF_A_FEM_ACTION_TYPE_MAP, FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_0_1_MAP,
 *   FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_2_15_MAP, FIELD_PMF_A_FEM_ADDER_FEMS_2_15_MAP
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] dnx_field_fem_action_info_p -
 *    Structure containing all info regarding fem action.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * Note that info on 'dnx_field_fem_action_info_p' corresponds to one 'FEM condition'
 * \see
 *   * dnx_field_actions_fem_log_to_phys_bit_source()
 *   * dnx_field_fem_action_info_t
 */
shr_error_e dnx_field_actions_fem_action_info_hw_set(
    int unit,
    dnx_field_fem_action_info_t * dnx_field_fem_action_info_p);
/**
 * \brief
 *   Read from hardware: For given 'FEM id' and 'map index' (contained within input structure) read
 *   all info corresponding to one 'FEM action' into a container structure.
 *   Note that 'map index' selects one of four actions available per one 'FEM id'
 *   Accessed HW tables are: IPPC_FEM_*_4B_MAP_TABLE, IPPC_FEM_*_24B_MAP_TABLE
 *   Accessed DBAL tables are: FIELD_PMF_A_FEM_ACTION_TYPE_MAP, FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_0_1_MAP,
 *   FIELD_PMF_A_FEM_FIELD_SELECT_FEMS_2_15_MAP, FIELD_PMF_A_FEM_ADDER_FEMS_2_15_MAP
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in,out] dnx_field_fem_action_info_p -
 *    As input:
 *      dnx_field_fem_action_info_p->fem_id -
 *        Identifier of the FEM for which 'action info' is required.
 *      dnx_field_fem_action_info_p->fem_map_index -
 *        Identifier of the 'action info' (one out of four, per 'fem id')
 *    As output:
 *      This procedure loads the rest of the 'dnx_field_fem_action_info_t' structure with
 *      all info regarding 'fem action'.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * Note that info on 'dnx_field_fem_action_info_p' corresponds to one 'FEM condition'
 * \see
 *   * dnx_field_actions_fem_log_to_phys_bit_source()
 *   * dnx_field_fem_action_info_t
 */
shr_error_e dnx_field_actions_fem_action_info_hw_get(
    int unit,
    dnx_field_fem_action_info_t * dnx_field_fem_action_info_p);

/*
 * }
 */

/*
 * }
 */
#endif
