/*
 **************************************************************************************
 Copyright 2012-2019 Broadcom Corporation

 This program is the proprietary software of Broadcom Corporation and/or its licensors,
 and may only be used, duplicated, modified or distributed pursuant to the terms and
 conditions of a separate, written license agreement executed between you and
 Broadcom (an "Authorized License").Except as set forth in an Authorized License,
 Broadcom grants no license (express or implied),right to use, or waiver of any kind
 with respect to the Software, and Broadcom expressly reserves all rights in and to
 the Software and all intellectual property rights therein.
 IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 WAY,AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization, constitutes the
    valuable trade secrets of Broadcom, and you shall use all reasonable efforts to
    protect the confidentiality thereof,and to use this information only in connection
    with your use of Broadcom integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" AND WITH
    ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER
    EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM
    SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
    NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS LICENSORS
    BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES
    WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE
    THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
    OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
    ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 **************************************************************************************
 */

#ifndef __KAPS_INSTRUCTION_INTERNAL_H
#define __KAPS_INSTRUCTION_INTERNAL_H

#include "kaps_instruction.h"
#include "kaps_list.h"
#include "kaps_hw_limits.h"
#include "kaps_db_wb.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @cond INTERNAL
 *
 * @file instruction_internal.h
 *
 * Structural details of instruction/ltrs
 */

    struct kaps_device;
    struct kaps_ab_info;

/**
 * @brief Specifies if the KBP entry is in decision trie or overflow
 */

    enum kaps_entry_status
    {
        PC_OVERFLOW_0 = 0,
                         /**< Power controlled overflow zero */
        PC_OVERFLOW_1 = 1,
                         /**< Power controlled overflow one */
        DECISION_TRIE_0 = 2,
                         /**< Entry is in decision trie zero */
        DECISION_TRIE_1 = 3,
                         /**< Entry is in decision trie one */
        OVERFLOW = 4,    /**< In overflow after being rejected
                            from all decision trees */
        UNHANDLED = 5    /**< No decisions made yet for the entry */
    };

/**
 * Convert list node to instruction
 */

#define KAPS_INSTLIST_TO_ENTRY(ptr) CONTAINER_OF((ptr), struct kaps_instruction, node)

/**
 * Instruction type
 */

    enum instruction_type
    {
        INSTR_UNKNOWN,
        INSTR_CBWLPM,       /**< Compare LPM instruction*/
        INSTR_COMPARE1,     /**< Compare1 Instruction*/
        INSTR_COMPARE2,     /**< Compare2 Instruction*/
        INSTR_COMPARE3      /**< Compare3 Instruction*/
    };

/**
 * Description of a search in an instruction
 */
    struct instruction_desc
    {
        struct kaps_db *db;              /**< Database being searched */
        uint32_t dba_res_map:16;        /**< 16b DBA result bitmap (OP) */
        uint8_t sram_dt_num[8];         /**< DT numbers assigned to this instruction desc */
        uint32_t kpu_id:3;              /**< KPU selected for this table */
        uint32_t result_id:3;           /**< Result Number */
        uint32_t user_result_id:3;      /**< User specified result number */
        uint32_t is_dup_required:1;     /**< this is a duplicated DB */
        uint32_t num_segments:4;        /**< Number of key segments */
        uint32_t num_ad_bytes:6;        /**< Number of AD bytes that go on wire for this search */
        uint32_t last_segment_width:8;  /**< Number of bytes in last segment */
        uint32_t last_segment_start_byte:8;
                                        /**< Start byte of last segment */
        uint32_t user_specified:1;      /**< User specified result data length for the particular result port */
    };

/**
 * Details of a single cycle search instruction
 */
    struct kaps_instruction
    {
        struct kaps_list_node node;      /**< For chaining instructions */
        struct kaps_device *device;  /**< device handle */
        struct kaps_key *master_key; /**< Master search key specification */
        uintptr_t stale_ptr;        /**< Pointer before ISSU */
        uintptr_t cascaded_inst_stale_ptr;        /**< Pointer before ISSU */
        struct instruction_desc desc[KAPS_INSTRUCTION_MAX_RESULTS];
                                                                /**< Description of the searches in an instruction*/
        uint32_t ltr:8;             /**< LTR number */
        uint32_t num_searches:4;    /**< Number of db/tables/clones being searched */
        uint32_t type:4;            /**< ::instruction_type */
        uint32_t is_installed:1;    /**< Has install been called on the device */
        uint32_t is_cmp3_pair:1;    /**< This LTR is compare3 pair in 12K */
        uint32_t has_resolution:1;  /**< Any Resolution to happen (OP) */
        uint32_t is_cmp3:1;         /**< Used for Compare3 compatibility (OP specific) */
        uint32_t is_cascading_src:1;/**< 1 means this instruction has been
                                         cascaded to another instruction in other core */
        uint32_t is_cascading_dst:1;/**< 1 means this instruction has been cascaded from
                                         another instruction in other core */
        struct kaps_instruction *cascaded_inst;
                                            /**< corresponding cascaded instruction in other core
                                                has been cascaded */
        uint32_t id;                /**< id */
        uint32_t lut_rec_length;    /**< lut rec length */
        uint32_t lut_index;         /**< LUT index for this instruction (OP/OP2 specific)*/
        uint8_t is_prop_set[KAPS_INSTRUCTION_MAX_RESULTS];     /**< Is property set*/
        uint8_t is_prop_set_xml_dump[KAPS_INSTRUCTION_MAX_RESULTS];
                                                                  /**< for XML dump purpose, keep a backup of previous is property set*/
        uint32_t prop_num_bytes[KAPS_INSTRUCTION_MAX_RESULTS];    /**< Prop Num bytes */
        uint32_t prop_byte_offset[KAPS_INSTRUCTION_MAX_RESULTS];  /**< Prop byte offset */
        uint8_t dba_dt_bit_map;     /**< DBA DT result MAP */
        uint8_t cmp3_result_port_shift;
                                    /**< Used for Compare3 compatibility (OP specific).
                                         Keeps track of the amount of shift made in
                                         result ports as part of SMT re-assignment*/
        uint32_t conflict_bits;     /**< conflict bits for OP corresponding to this instruction,
                                      bit no 0 to 3 internal use,
                                      bit no 4 to 7 represents overflow
                                      bit no 8 to 11 represents result ports
                                      bit no 12 to 15 represents KPU */

                                    /**< conflict bits for OP2 corresponding to this instruction,
                                      bit no 0 to 7 internal use,
                                      bit no 8 to 15 represents overflow
                                      bit no 16 to 23 represents result ports
                                      bit no 24 to 27 represents KPU */
    };

/**
 * Number of cycles taken for the given instruction on the device
 *
 * @param device valid initialized KBP device handle
 * @param instr valid instruction of type ::instruction_type
 * @param clocks the number of core clocks taken
 *
 * @return KAPS_OK on success, in which case the clocks are valid
 */

    kaps_status kaps_instruction_clocks(
    struct kaps_device *device,
    enum instruction_type instr,
    uint32_t * clocks);

/**
 * Return printable name for instruction
 *
 * @param device valid initialized KBP device handle
 * @param instr valid instruction of type ::instruction_type
 * @param name printable name
 *
 * @return KAPS_OK on success, in which case the name is valid
 */

    kaps_status kaps_instruction_name(
    struct kaps_device *device,
    enum instruction_type instr,
    char **name);

/**
 * Initialize the instruction to defaults for 12k. This
 * is post resource management pass
 *
 * @param instruction the instruction t0 finalize
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_instruction_finalize_12k(
    struct kaps_instruction *instruction);

/**
 * Initialize the instruction to defaults for Optimus Prime. This
 * is post resource management pass.
 *
 * @param instruction the instruction t finalize
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_instruction_finalize_op(
    struct kaps_instruction *instruction);

/**
 * Advanced finalization of instruction
 *
 * @param instruction the instruction t finalize
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_instruction_finalize_advanced(
    struct kaps_instruction *instruction);

/**
 * Determine the type of the instruction
 *
 * @param instruction the instruction to check
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_instruction_determine_type(
    struct kaps_instruction *instruction);

/**
 * Format and check the instruction for 12K
 *
 * @param instruction the instruction to check
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_instruction_install_12k(
    struct kaps_instruction *instruction);

/**
 * Format and check the instruction for 12K
 *
 * @param instruction the instruction to check
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_instruction_install_op(
    struct kaps_instruction *instruction);

/**
 * Writes the conflict bits of an instruction
 * into LTR register
 *
 * @param device valid device pointer
 * @param instr the instruction to check
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status write_op_conflict_bits(
    struct kaps_device *device,
    struct kaps_instruction *instr);

/**
 * Save the instruction related information for warmboot
 *
 * @param instruction the instruction to save
 * @param wb_fun WB function state
 *
 * @return KAPS_OK on success or error code
 */

    kaps_status kaps_instruction_wb_save(
    struct kaps_instruction *instruction,
    struct kaps_wb_cb_functions *wb_fun);

/**
 * Restore the instruction related information for warmboot
 *
 * @param device valid device pointer
 * @param wb_fun WB function state
 *
 * @return KAPS_OK on success or error code
 */

    kaps_status kaps_instruction_wb_restore(
    struct kaps_device *device,
    struct kaps_wb_cb_functions *wb_fun);

/**
 * For all the instruction this database participates in
 * activate the specified AB. This only enables the AB
 * select and super block select in the relevant instructions.
 * When cascade is specified, the AB is only validated
 * in the instructions on the device on which the AB resides.
 *
 * @param db valid database handle
 * @param ab_info the DBA block AB information
 * @param type the decision tree type
 * @param force_res_port if force_res_port is not -1,
 *        then the block result port is set to force_res_port
 *
 * @return KAPS_OK on success or error code
 */

    kaps_status kaps_instruction_enable_ab_for_db(
    struct kaps_db *db,
    struct kaps_ab_info *ab_info,
    enum kaps_entry_status type,
    int32_t force_res_port);

/**
 * For all the instruction this database participates in
 * de-activate the specified AB. This only disables the AB
 * select and super block select in the relevant instructions.
 * When cascade is specified, the AB is only invalidated
 * in the instructions on the device on which the AB resides.
 *
 * @param db valid database handle
 * @param ab_info the DBA block AB information
 * @param type the decision tree type
 * @param is_blk_empty indicates if the block is completely empty.
 *        This param is needed since the block may be
 *        shared by multiple databases. So in some cases the
 *        db may no longer reside in the block, but entries of other
 *        dbs may still be present in the block
 *
 * @return KAPS_OK on success or error code
 */

    kaps_status kaps_instruction_disable_ab_for_db(
    struct kaps_db *db,
    struct kaps_ab_info *ab_info,
    enum kaps_entry_status type,
    int32_t is_blk_empty);

/**
 * @brief Range extraction information from master key
 */
    struct kaps_range_extraction
    {
        uint16_t encoding;    /**< Of type ::optimus_range_ctrl */
        uint16_t extraction_byte;
                              /**< Extraction start byte */
    };

/**
 * Optimus Prime specific device writes to range extraction register
 *
 * @param device valid device handle
 * @param ltr_num the LTR number
 * @param ext_info The extraction information
 *
 * @return KAPS_OK on success or error code
 */

    kaps_status kaps_instruction_op_write_range_extraction(
    struct kaps_instruction *instruction,
    struct kaps_range_extraction *ext_info);

/**
 * 12k specific device writes to range extraction register
 *
 * @param device valid device handle
 * @param ltr_num the LTR number
 * @param ext_info The extraction information
 *
 * @return KAPS_OK on success or error code
 */

    kaps_status kaps_instruction_12k_write_range_extraction(
    struct kaps_instruction *instruction,
    struct kaps_range_extraction *ext_info);

/**
 * Configure range extraction information for the instruction
 *
 * @param instruction valid instruction handle
 *
 * @ KAPS_OK on success or error code
 */

    kaps_status kaps_instruction_configure_range_extraction(
    struct kaps_instruction *instruction);

/**
 * Parses an instruction from a ISSU dump file and
 * Outputs it to a file in human readable format
 *
 * @param bin_fp File pointer of the ISSU dump file
 * @param txt_fp File pointer of the Output file
 * @param nv_offset offset of the ISSU dump file
 *
 * @retval KAPS_OK on succes
 */
    kaps_status kaps_instruction_wb_print(
    FILE * bin_fp,
    FILE * txt_fp,
    uint32_t * nv_offset);

/**
 * Finalize the KPU allocation.
 *
 * @param device the KBP device handle
 *
 * @return KAPS_OK on success
 */

    kaps_status kaps_instruction_finalize_kpus(
    struct kaps_device *device);

/**
 * Calculate the number of segments required for each db in the instruction
 *
 * @param instruction valid instruction handle
 *
 * @ KAPS_OK on success or error code
 */

    kaps_status kaps_instruction_calc_key_segments(
    struct kaps_instruction *instruction);

/**
 * This function checks the max interface width (specific to op)
 *
 * @param instruction valid instruction handle
 *
 * @ KAPS_OK on success or error code
 */

    kaps_status kaps_instruction_check_interface_width_op(
    struct kaps_instruction *instruction);

/**
 * Calculates the conflict bits of an instruction.
 *
 * @param device the KBP device handle
 * @param instruction valid instruction handle
 *
 * @return KAPS_OK on success
 */

    kaps_status kaps_device_inst_calculate_conflict_bits(
    struct kaps_device *main_device,
    struct kaps_instruction *instruction);

/**
 * Cascades an instruction into other core
 *
 * @param instr valid instruction handle
 * @param from_device the KBP device handle from which the instruction has to be cascaded
 * @param to_device the KBP device handle to which the instruction has to be cascaded
 *
 * @return KAPS_OK on success
 */
    kaps_status cascade_instruction(
    struct kaps_instruction *instr,
    struct kaps_device *from_device,
    struct kaps_device *to_device,
    struct kaps_c_list_iter *it);

/**
 * Finds the max AD width on a particular result port in an instruction
 *
 * @param instr instruction handle in which max_ad_width needs to be find out
 * @param result_id the result_id number in which max_ad_width needs to be find out
 *
 * @return KAPS_OK on success
 */
    int32_t get_highest_ad_width(
    struct kaps_instruction *instr,
    int32_t result_id);

/**
 * @endcond
 */

#ifdef __cplusplus
}
#endif
#endif  /*__INSTRUCTION_INTERNAL_H */
