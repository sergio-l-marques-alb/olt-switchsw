/*******************************************************************************
 *
 * Copyright 2011-2019 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate, written license agreement executed
 * between you and Broadcom (an "Authorized License").  Except as set forth in an
 * Authorized License, Broadcom grants no license (express or implied), right to
 * use, or waiver of any kind with respect to the Software, and Broadcom expressly
 * reserves all rights in and to the Software and all intellectual property rights
 * therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS
 * SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use all
 * reasonable efforts to protect the confidentiality thereof, and to use this
 * information only in connection with your use of Broadcom integrated circuit
 * products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
 * OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
 * TO THE SOFTWARE. BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
 * OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
 * OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 *******************************************************************************/

#ifndef __KAPS_ALGO_HW_H
#define __KAPS_ALGO_HW_H

#include "kaps_device_internal.h"
#include "kaps_resource_algo.h"
#include "kaps_device_alg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @cond INTERNAL
 */

/**
 * @file algo_hw.h
 *
 * Internal details and structures used for writing to algorithmic portions of hardware
 *
 */

/**
 * @addtogroup ACL_ALGO_HW
 * @{
 */

/**
 * @brief Power control table information to be written to hardware
 */

    struct kaps_pct
    {
        union
        {
            struct kaps_ads kaps;
        } u;
    };

/**
* Hidden Device properties
*/

    enum kaps_device_advanced_properties
    {
        KAPS_HW_RESOURCE_DBA_BMP = 666,      /**< Bitmap of ABs to be used */
        KAPS_HW_RESOURCE_UDA_BMP,            /**< Bitmap of UDA 8Mb blocks to be used. */
        KAPS_DEVICE_PROP_ADV_PRINT,          /**< Enables the Device Mapping print*/
        KAPS_DEVICE_PROP_LAST_RPT_PARITY_ERRORS,
                                             /**< Last seen RPT parity error address of specific device
                                                 [updated by kaps_device_advanced_fix_errors] */
        KAPS_DEVICE_PROP_INJECT_RPT_PARITY_ERRORS,  /**< inject the RPT errors by using the Netl Regs
                                                      [by kaps_device_inject_adv_parity_error] */
        KAPS_DEVICE_PROP_ENABLE_DISABLE_RPT_PARITY_SCAN,
                                                     /**< Enable or Disable the RPT scanning
                                                      [by kaps_device_adv_parity_scan_enable_disable] */
        KAPS_DEVICE_ADV_UDA_WRITE,          /**< Same as KAPS_DEVICE_PROP_ADV_UDA_WRITE (retained for backward compatibility) */
        KAPS_DEVICE_PROP_POWER_BUDGET_UDA,  /**< limits the max no of UDMs that can be active at any cycle */
        KAPS_DEVICE_PROP_POWER_BUDGET_DBA,  /**< limits the max no of UDMs that can be active at any cycle */
        KAPS_DEVICE_PROP_REDUCE_NUMBER_OF_ROWS,
                                            /**< Reduces the Number of Rows in AB and LPU */
        KAPS_DEVICE_PROP_NO_COMPRESSION,    /**< Turns of DBA Trigger Compression */
        KAPS_DEVICE_ADV_PROP_XML_DUMP_FMT,  /**< Whether to dump the XML with (1) or without (0) the entries. */
        KAPS_DEVICE_PROP_NO_OVERFLOW_LPM,   /**< LPM without Overflow (without APT) */
        KAPS_DEVICE_PROP_LPT_MODE,          /**< 1: enable LPT, 0:disable LPT. Enabled by default */
        KAPS_DEVICE_PROP_LAST_UIT_PARITY_ERRORS,
                                            /**< Last seen UIT parity error address of specific device
                                                 [updated by kaps_device_advanced_fix_errors] */
        KAPS_DEVICE_PROP_LOAD_FW,           /**< Load FW for ARM. Valid only for OP */
        KAPS_DEVICE_PROP_LAST_1B_PARITY_ERRORS,
                                            /**< LAST seen 1b parity locs: OP 0:DBA, 1:UDA, 2:UIT */
        KAPS_DEVICE_PROP_NUM_AB_AVAILABLE,  /**< Number of AB's available */
        KAPS_DEVICE_PROP_NUM_UDA_AVAILABLE, /**< How much UDA is available */
        KAPS_DEVICE_PROP_ADV_DYNAMIC_ALLOC_PRINT,
                                             /**< Enables the Dynamic allocation print*/
        KAPS_DEVICE_PROP_INJECT_MEMORY_ERRORS = 750,
                                                 /**< inject error on specified locations for given memory type DBA/UDA/UIT/RPT*/
        KAPS_DEVICE_PROP_DISABLE_INST_CASCADE,
                                             /**< op2 specific to disable the instruction cascading*/
        KAPS_DEVICE_ADV_PROP_INVALID         /**< This should be the last entry. */
    };

/**
 * Device resource information
 */

    struct kaps_device_resource
    {
        struct kaps_simple_dba *pcm_dba_mgr[HW_MAX_PCM_BLOCKS];
                                                            /**< DBA manager for RPT blocks.
                                                         And it is common for all databases that
                                                         are present in respective RPT blocks */
        uint16_t num_rpt_blocks;
                             /**< Number of RPT/PCT blocks present in the device */
        FILE *vector_logger;/**< File pointer of model vector logging */
        uint16_t dba_width_1;
                            /**< Max DBA entry width in bits */
        uint16_t sram_dt_per_db;
                             /**< Number of SRAM based decision trees */
        uint16_t dba_dt_per_db;
                             /**< Number of DBA based decision trees */
        uint16_t total_lpus;/**< The number of LPUs on device */
        uint16_t max_lpu_per_db;
                            /**< Max LPUs per database */
        uint32_t rows_in_lpu;
                            /**< Number of bricks in LPU */
        uint32_t rows_reached_by_lpu;
                                  /**< Max number of bricks an LPU unit can address */
        uint16_t lpu_word_size_8;
                             /**< Word size granularity in LPU in bytes */

        uint16_t indirection_width_1;
                                  /**< Indirection pointer width in bits */

        uint16_t pc_width_8;/**< Width of power control entries */

        uint16_t pc_depth;  /**< Number of power control entries in one rpt/pct */
        uint16_t total_num_bb;
                            /**< Total number of Bucket Blocks in the device. relevant only for KAPS */
        uint16_t num_chained_bb;
                             /**< Total number of Bucket Blocks chained to form a cascade. relevant only for KAPS */
        uint16_t bb_width_1; /**< Width of Bucket Blocks in bits. relevant only for KAPS */
        uint16_t total_small_bb;
                             /**< Number of Small Bucket Blocks, Relevant only for KAPS*/
        uint16_t num_rows_in_small_bb;
                                   /**< Number of rows in the Small Bucket Block. Relevant only for KAPS*/
        uint16_t lpm_gran_array[KAPS_HW_MAX_NUM_LPU_GRAN];
                                                       /**< The granularities supported by the device for LPM */
        uint16_t lpm_middle_level_gran_array[KAPS_HW_MAX_NUM_LPU_GRAN];
                                                                    /**< The granularities supported by the device for LPM for the middle level*/

        uint16_t lpm_num_gran;
                            /**< The number of LPM granularities supported by the device */
        uint16_t lpm_middle_level_num_gran;
                                        /**< The number of LPM granularities supported by the device in the middle level*/

        uint32_t incr_in_bbs;    /**< Number of BB's to grow dynamically */
        uint32_t has_algorithmic_lpm:1;
                                    /**< Supports algorithmic LPMs */
        uint32_t device_print_advance:2;
                                     /**< Enables the Device Mapping print */
        uint32_t no_dba_compression:1;
                                   /**< Turns off DBA trigger Compression */
        uint32_t no_overflow_lpm:1; /**< Turns on LPM without Overflow (without APT)*/
        uint32_t disable_header_print:1;
                                     /**< when set, the device print api only prints the device mapping info */

        uint32_t num_algo_levels:2;
                                /**< Number of algorithmic levels (2 or 3) for this device */
        uint32_t reduced_algo_levels:1;
                                    /**< If set to 1, we are using only 2 algorithmic levels in a device that supports 3 algorithmic levels*/

        void *it_mgr;            /**< Opaque IT manager pointer */
        resource_process_fn process_fn;
                                   /**< Process files for capacity */
        void *res_hdl;           /**< handle for above callback */

        uint64_t running_sum;
                          /**< Running sum of the bytes of the entries added to the databases on the device*/

        uint32_t ads2_depth_a;  /**< Depth of the ADS2 for type a database */
        uint32_t ads2_depth_b;  /**< Depth of the ADS2 for type b database */

    };

/**
 * Write an entry to the Root Pivot Table (RPT) for all the KPUs
 *
 * @param device valid device handle
 * @param rpt_num RPT block number
 * @param data  pointer to 320 bit data that should be written to RPT
 * @param mask pointer to 320 bit mask that should be written to RPT
 * @param t entry_nr location of the entry in the RPT
 * @param type the decision tree information
 * @param is_xy whether data and mask are in XY mode
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_algo_hw_write_rpt_data(
    struct kaps_device *device,
    uint8_t rpt_num,
    uint8_t * data,
    uint8_t * mask,
    uint16_t entry_nr,
    uint8_t is_xy);

/**
 * Delete an entry to the Root Pivot Table (RPT) for all the KPUs
 *
 * @param device valid device handle
 * @param rpt_num RPT block number
 * @param t entry_nr location of the entry in the RPT to be deleted
 * @param type the decision tree information
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_algo_hw_delete_rpt_data(
    struct kaps_device *device,
    uint8_t rpt_num,
    uint16_t entry_nr);

/**
 * Write an entry to the Power Control Table (the information table for the RPT entry) for all the keys
 *
 * @param device valid device handle
 * @param pct fields to be written to the power control table
 * @param pct_num PCT block number
 * @param entry_nr location of the entry in the PCT
 * @param type the decision tree information
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_algo_hw_write_pct(
    struct kaps_device *device,
    struct kaps_pct *pct,
    uint8_t pct_num,
    uint16_t entry_nr);

/**
 * Write an entry to the RPT Block Config Register
 *
 * @param device valid device handle
 * @param enable indicates if the RPT Block should be enabled or not
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status algo_hw_write_rpt_block_config(
    struct kaps_device *device,
    uint8_t enable);

/**
 * Power up RPT blocks assigned to databases
 *
 * @param device valid device handle
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status algo_hw_power_up_rpt(
    struct kaps_device *device);

/**
 * Initialize the RPT and UIT blocks
 *
 * @param device valid device handle
 *
 * @return KAPS_OK on success or error code
 */

    kaps_status algo_hw_init_rpt_uit(
    struct kaps_device *device);

/**
 * Advanced instruction finalization for Optimus Prime
 *
 * @param instruction the instruction t finalize
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_instruction_finalize_op_adv(
    struct kaps_instruction *instruction);

/**
 * update less_udc_pairs at op_srch_attr_ctrl_alg which got changed due to dynamic allocation
 * @param device valid device handle
 * @param db valid db handle
 * @param end_udc the last udc of the dynamically allocated region
 * @return KAPS_OK on success or error code
*/
    kaps_status write_op_srch_attr_ctrl_dynamic(
    struct kaps_device *device,
    struct kaps_db *db,
    uint8_t end_udc);

/**
 * update the op_lpu_ctrl register for the dynamically allocated UDM's
 * @param device valid device handle
 * @param db valid db handle
 * @param start_row start row no of the dynamically allocated region
 * @param start_lpu start lpu no of the dynamically allocated region
 * @param num_row no of rows in the dynamically allocated region
 * @param num_lpu no of lpu in the dynamically allocated region
 * @param dt_index decision tree index for which this allocation is done
 * @return KAPS_OK on success or error code
 */
    kaps_status write_op_udc_ctrl_dynamic(
    struct kaps_device *device,
    struct kaps_db *db,
    int32_t start_row,
    int32_t start_lpu,
    int32_t num_row,
    int32_t num_lpu,
    int32_t dt_index);

/**
 * Advanced instruction finalization for 12K
 *
 * @param instruction the instruction t finalize
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_instruction_finalize_12k_adv(
    struct kaps_instruction *instruction);

/**
 * Enable the UDM, and mark it for LSN
 *
 * @param device valid device handle
 * @param udmno the UDM number to enable
 * @param is_lsn if this UDM is being used for LSN
 * @param is_xor_ad if this UDM is being used for XOR AD
 * @param is_udm_pair whether to light up the mirrored udm for >320bit NetACL
 * @param enable_udm 1: Power up UDM, 0: Power down UDM
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_device_advanced_enable_udm(
    struct kaps_device *device,
    uint32_t udmno,
    int32_t is_lsn,
    int32_t is_xor_ad,
    int32_t is_udm_pair,
    uint8_t enable_udm);

/**
 * Enable UDC for XOR LSN processing
 *
 * @param device valid device handle
 * @param udcno the UDC number to enable
 * @param enable_xor 1: XOR on, 0: XOR off
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_device_advanced_enable_udc_xor(
    struct kaps_device *device,
    uint32_t udcno,
    uint8_t enable_xor);

/**
 * API that saves the shadow memory of KAPS device during warmboot
 *
 * @param device valid device handle
 * @param wb_fun WB state
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_device_save_kaps_shadow(
    struct kaps_device *device,
    struct kaps_wb_cb_functions *wb_fun);

/**
 * API that restores the shadow memory of KAPS device during warmboot
 *
 * @param device valid device handle
 * @param wb_fun WB state
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_device_restore_kaps_shadow(
    struct kaps_device *device,
    struct kaps_wb_cb_functions *wb_fun);

/**
 * Allocates a SRAM DT dynamically
 *
 * @param db_main valid database handle
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_device_alloc_dt_dynamic(
    struct kaps_db *db_main);

/**
 * Deletes a SRAM DT dynamically
 *
 * @param db_main valid database handle
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_device_delete_dt_dynamic(
    struct kaps_db *db_main);

/**
 * API to get the LPM memory stats
 *
 * @param device valid device handle
 * @param fp valid file pointer
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_device_lpm_memory_stats(
    struct kaps_device *device,
    FILE * fp);

/**
 * @}
 */

/**
 * @endcond
 */

#ifdef __cplusplus
}
#endif
#endif
