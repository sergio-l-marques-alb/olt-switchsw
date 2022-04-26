/*******************************************************************************
 *
 * Copyright 2012-2019 Broadcom Corporation
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

#ifndef __KAPS_RESOURCE_H
#define __KAPS_RESOURCE_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include "kaps_device.h"
#include "kaps_hw_limits.h"
#include "kaps_errors.h"
#include "kaps_list.h"
#include "kaps_bitmap.h"



#ifdef __cplusplus
extern "C"
{
#endif

    struct kaps_db;
    struct kaps_instruction;
    struct kaps_device;
    struct kaps_key;
    struct memory_map;
    struct kaps_ab_info;
    struct kaps_ad_db;

/**
 * Maximum Power controlled branches for one DB
 */

#define MAX_PC_BRANCHES_PER_DB 4

/**
 * Invalid AB number
 */

#define INVALID_AB (0xFFFFFFFF)

/**
 * Failed to allocate DBA resources
 */

#define FAILED_FIT_DBA  1

/**
 * Failed to allocate AD memory
 */

#define FAILED_FIT_AD   2

/**
 * Value of bitmap if UDM is fully occupied
 */
#define UDM_FULLY_OCCUPIED 0xF

/**
 * Maximum number of owners of a single super block
 */

#define MAX_DB_PER_SB 8

/**
 * Maximum number of AD banks (only for 12K)
 */

#define MAX_AD_BANKS 2

    struct kaps_db_hw_resource
    {
        int8_t ad_type;  /**< ::kaps_ad_type */
        struct kaps_c_list ab_list;
                                  /**< List of ABs available for this database */
        struct kaps_c_list cas_ab_list;
                                   /**< List of ABs available for this database on other core*/

        struct kaps_c_list rpb_ab_list; /**< List of RPB Blocks to be used by 2 level database from JR2 onwards*/                            

        int8_t start_mcor;
                         /**< First MCOR unit */
        int8_t end_mcor; /**< End MCOR unit (inclusive) */
        uint16_t kpu_bmp;/**< Indicates in which all KPU this database is scheduled (12K specific) */
        const char *color;
                         /**< Random color for XML output */
        uint8_t user_specified_algo_type;
                                      /**< is user specified the algo mode */

        struct
        {
            uint8_t alloc_udm[KAPS_MAX_NUM_CORES][KAPS_UDM_PER_UDC][KAPS_ALLOC_UDM_MAX_COLS];
                                                                                           /**< Which all UDMs has been allocated to this DT */
            uint8_t max_lsn_size;         /**< maximum LSN size for this DT*/
            uint8_t max_lsn_size_budget;  /**< max lsn_size_permitted to satisfy the power budget */
            uint16_t uda_mb;              /**< amount of UDA to be allocated for this DT*/
            uint8_t combined_lsn_size;    /**< increased LSN size due to common UDA mgr */
            uint16_t combined_uda_mb;     /**< combined UDA mb of the common UDA mgr */
        } lsn_info[KAPS_HW_MAX_DT_PER_DB]; /**< LSN info per DT */

        struct
        {
            uint8_t rpt_bmp[HW_MAX_PCM_BLOCKS / KAPS_BITS_IN_BYTE];
                                                                /**< 16b RPT bitmap */
            uint8_t dba_bmp[KAPS_HW_MAX_AB / KAPS_BITS_IN_BYTE];
                                                             /**< 256b DBA bitmap */
            uint8_t num_dba_dt;
                            /**< Number of DBA based decision trees */
            uint8_t num_sram_dt;
                             /**< Number of SRAM based decision tress */
            uint8_t max_lsn_size[MAX_PC_BRANCHES_PER_DB];
                                                      /**< Maximum LSN size of each DT */
        } user_specified;
                      /**< User input specification */
        uint32_t start_ait_sb;             /**< AIT start UDA Super Block number for NLA12K*/
        uint32_t num_ait_sb;               /**< Number of AIT UDA Super Blocks allocated for NLA12K*/
        uint32_t start_lpu;                /**< DMA Start LPU for KAPS*/
        uint32_t end_lpu;                  /**< DMA End LPU for KAPS*/
        uint32_t start_small_bb_nr;        /**< Start Small BB number for 3 level KAPS*/
        uint32_t end_small_bb_nr;          /**< End Small BB number for 3 level KAPS*/
        uint32_t num_small_bb_per_row;     /**< Number of small BB in one row assigned to the database 3 level KAPS*/
        uint32_t max_num_bricks_per_ab;        /**< Maximum Number of Bricks per AB for 3 level KAPS*/
        uint32_t small_bb_brick_bitmap_num_bytes; /**< Number of bytes in the Small BB Brick Bitmap*/

        uint8_t *small_bb_brick_bitmap;
                                                         /**< Bitmap indicating which small BB bricks are free and which are occupied
                                                                                            The bitmap starts from start_small_bb_nr of the db and not always from small bb 0.
                                                                                            First the bitmap of row 0 from start_small_bb_nr to end_small_bb_nr is present, followed by row 1, etc*/
        uint16_t rpt_map[MAX_PC_BRANCHES_PER_DB];
                                                /**< 16-bit RPT bitmap for each power controlled branch (Optimus only) */
        uint8_t rpt_width_8[MAX_PC_BRANCHES_PER_DB];/**< RPT width for each power controlled branch */
        uint32_t dba_budget:9;              /**< DBA power budget for this database (Optimus only) */
        uint32_t num_dt:4;   /**< Number of decision trees */
        uint32_t num_allocated_dt:3;   /**< Number of decision trees allocated so far*/
        uint32_t num_dba_dt:4;
                             /**< Number of DBA based decision trees */
        uint32_t num_dba_dt_user:4;
                                 /**< Backup of user given number of DBA based decision trees */
        uint32_t algorithmic:1;      /**< If set, can use algorithmic search */
        uint32_t num_bbs:8;  /**< Number of bucket blocks (KAPS Only) */
      
    };

/*type of AB to allocate*/
    typedef enum
    {
        AB_ANY,
        AB_NORMAL,
        AB_SMALL
    } ab_type_t;



/**
 * The various ways associated data may be handled
 */

    enum kaps_ad_type
    {
        KAPS_AD_TYPE_NONE,                     /**< No Associated data */
        KAPS_AD_TYPE_CUSTOM,                   /**< Custom index translation, not really AD */
        KAPS_AD_TYPE_INDEX,                    /**< Index based AD, AD is shuffled with entry */
        KAPS_AD_TYPE_INDIRECTION,              /**< Indirection based AD, zero shuffles */
        KAPS_AD_TYPE_INDIRECTION_WITH_DUPLICATION,
                                               /**< Indirection based AD with duplication */
        KAPS_AD_TYPE_INPLACE                   /**< In place AD, shuffled with entry */
    };

/**
 * Represents a single 8Mb chunk in HW (UDM)
 */

    struct udm_info
    {
        struct kaps_db *db[KAPS_HW_NUM_SB_PER_UDM];
                                                /**< Database whose entries are stored in the UDA Super Block, NULL means free */
        uint32_t occupied_bmp:4;
                              /**< maximum of four bits can be set 0xF means whole UDM is used */
        uint32_t ad_or_it_bmp:4;
                              /**< Four bits bitmap. If the UDM is not used for LSN and any bit is set,
                                   that UDA SB is used for AIT (12K specific), if not it's AD. Ignored for OP */
        uint32_t is_xor:1;    /**< If the whole UDM is XOR, the whole UDM is used by one database */
        uint32_t is_lsn:1;    /**< All four SBs of the UDM are assigned to database for LSN processing */
        uint32_t dt_index:3;  /** which decision tree this UDM has been assigned to */
        uint32_t is_dynamic:1;  /* whether allocated dynamically. This will be useful in the print function to mark the 
                                 * dynamic allocation */
        uint32_t use_only_for_ad:1;     /* for palladium testing, if this is set then the UDM should not be used for
                                         * LSN */
        uint32_t is_reserved:1; /* This UDM is reserved */
    };

/**
 * Represents a Partition of the Bucket Block
 */

    struct bb_partition_info
    {
        uint32_t start_row;        /**< Start row of the partition*/
        uint32_t num_rows;         /**< Number of rows in the partition*/
        struct kaps_db *owner;      /**< Pointer to the database owing this set of rows */
        struct bb_partition_info *next;
                                    /**< Next partition of the bucket block */
    };

    enum large_bb_config_type
    {
        KAPS_LARGE_BB_WIDTH_A000_B000 = 0x1,
        KAPS_LARGE_BB_WIDTH_A000_B480 = 0x2,
        KAPS_LARGE_BB_WIDTH_A000_B960 = 0x4,
        KAPS_LARGE_BB_WIDTH_A480_B000 = 0x8,
        KAPS_LARGE_BB_WIDTH_A480_B480 = 0x10,
        KAPS_LARGE_BB_WIDTH_A960_B000 = 0x20
    };

/**
 * Represents a Bucket Block (1K Rows, 480b * num_bb_in_one_chain wide)
 */

    struct bb_info
    {
        uint32_t width_1;          /**< Width of the bucket blocks*/
        struct bb_partition_info *row;
                                    /**< Partitions of the bucket block */
        enum large_bb_config_type bb_config;
                                         /**< Chosen configuration for the BB. This is valid only for even BB numbers*/
        uint32_t bb_config_options;
                                /**< Options for configuring the BB*/
        uint32_t bb_num_rows;
                          /**< Total number of rows in the BB*/
        uint32_t num_units_in_bb;
                              /**< If different BBs have different number of rows, then we will split the BB into multiple units*/
    };





/**
 * Power control module ownership information
 */
    struct kaps_rpb_info
    {
        uint16_t rpt_width;
                        /**< RPT block width */
        uint16_t rpt_depth;
                        /**< Depth of RPT block */
        struct kaps_db *db;
                            /**< pointer to the database, assigned this RPT block */
    };



/**
 * Information about the entries stored in the UDA and DBA
 */

    struct memory_map
    {
        struct kaps_ab_info *ab_memory;          /**< AB information in the device */
        struct kaps_ab_info *rpb_ab;             /**< For 2 level JR2 databases, RPB mapped to AB */

        struct kaps_rpb_info rpb_info[HW_MAX_PCM_BLOCKS];  /**< RPB ownership information */

        uint8_t num_bb;                          /**< Number of  Bucket Blocks, KAPS specific */
        struct bb_info *bb_map[KAPS_HW_MAX_NUM_DB_GROUPS]; /**< array of Bucket Blocks */


        uint8_t num_small_bb;                    /**< Number of Small Bucket Blocks, KAPS specific */
        struct bb_info *small_bb;                /**< Array of Small bucket blocks */
    };

/**
* Linked list of SBs for Hitbit AD DB
*/
    struct kaps_hb_bank_list
    {
        uint8_t start_sb;           /**< Starting AD super block for the Hitbit bank */
        uint8_t num_sb;             /**< Number of contiguous super blocks */
        uint8_t bank_no;            /**< Hitbit Bank number */
        uint32_t num_idx;           /**< Number of entries in this Hitbit block */
        struct kaps_hb_bank_list *next;
                                     /**< Next Hitbit bank in this AD database */
    };

/**
Hitbit HW Resource
*/
    struct kaps_hb_db_hw_resource
    {
        struct kaps_hb_bank_list *hb_bank_list;
                                             /**< Hitbit Bank info */
    };

/**
 * Hardware resource information that will be used by AD databases
 */
    struct kaps_ad_db_hw_resource
    {
        enum kaps_ad_type ad_type;  /**< ::kaps_ad_type */
        int8_t dynamic_ad_chunk;
                             /**< The AD chunks will be allocated dynamically */

        struct kaps_hb_db_hw_resource *hb_res;
                                           /**< Assigned HW resources for HB databases */
    };

/**
 * Initialize resource management module
 *
 * @param device the KBP device handle
 */

    kaps_status kaps_resource_init_module(
    struct kaps_device *device);

/**
 * Initializes the memory map for the device
 *
 * @param device the valid device handle whose memory map should be initialized
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_resource_init_mem_map(
    struct kaps_device *main_device);

/**
 * Returns the resource map for the device
 *
 * @param device the valid device handle
 *
 * @return memory map of the device
 */
    struct memory_map *resource_kaps_get_memory_map(
    struct kaps_device *device);

/**
 * Returns the database handle that maintains the resources for the requesting database
 *
 * @param db requesting database handle
 *
 * @return database handle which maintains the resources for the requesting database
 */
    struct kaps_db *kaps_resource_get_res_db(
    struct kaps_db *db);

/**
 * Destroy the resource management module
 *
 * @param device the KBP device handle
 */

    void kaps_resource_fini_module(
    struct kaps_device *device);

/**
 * Initialize and add a database for resource management
 *
 * @param device the KBP device handle
 * @param db the database handle, initialized and returned on success
 * @param id the database ID
 * @param capacity the number of entries in the database
 * @param width_1 width of associated data entries. Valid for AD databases only
 * @param type the type of the database
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_resource_add_database(
    struct kaps_device *device,
    struct kaps_db *db,
    uint32_t id,
    uint32_t capacity,
    uint32_t width_1,
    enum kaps_db_type type);

/**
 * Free up any resource management information for this database
 *
 * @param device the KBP device handle
 * @param db valid database handle
 */

    void kaps_resource_free_database(
    struct kaps_device *dev,
    struct kaps_db *db);

/**
 * Add the key structure to the database
 *
 * @param device the KBP device handle
 * @param db the database handle
 * @param key the finalized kaps_key() structure
 */

    kaps_status kaps_resource_db_set_key(
    struct kaps_device *device,
    struct kaps_db *db,
    struct kaps_key *key);

/**
 * Add associated data to the database
 *
 * @param device the KBP device handle
 * @param db the database handle
 * @param ad the AD database
 *
 * @returns KAPS_OK on success or an error code
 */

    kaps_status kaps_resource_db_add_ad(
    struct kaps_device *device,
    struct kaps_db *db,
    struct kaps_db *ad);

/**
 * Add HB to the database
 *
 * @param device the KBP device handle
 * @param db the database handle
 * @param hb_db the HB database
 *
 * @returns KAPS_OK on success or an error code
 */

    kaps_status kaps_resource_db_add_hb(
    struct kaps_device *device,
    struct kaps_db *db,
    struct kaps_db *hb_db);



/**
 * Output the resource management information to html file
 *
 * @param device the KBP device handle
 * @param fp the file to dump HTML output to
 */

    void kaps_resource_print_html(
    struct kaps_device *device,
    FILE * fp);

/**
 * Finalize the resources required by the databases
 *
 * @param device the KBP device handle
 *
 * @return KAPS_OK on success or an resource management error.
 *         The output HTML can be used to print verbose information
 */

    kaps_status kaps_resource_finalize(
    struct kaps_device *device);

/**
 * Normalize AD width to that supported by HW
 *
 * @param device the KBP device handle
 * @param ad_width_1 the width in bits to be normalized
 *
 * @retval normalized width
 */

    uint32_t kaps_resource_normalized_ad_width(
    struct kaps_device *device,
    uint32_t ad_width_1);

/**
 * Normalize AD width that come out from response
 *
 * @param device the KBP device handle
 * @param ad_width_1 the width in bits to be normalized
 *
 * @retval normalized width
 */

    uint32_t resource_op_get_ad_width_in_response(
    struct kaps_device *device,
    uint32_t ad_width_1);


/**
 * Set the algorithmic properties of the database
 *
 * @param device KBP device handle
 * @param db the database to turn non-algorithmic
 * @param value the algorithmic number
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_resource_set_algorithmic(
    struct kaps_device *device,
    struct kaps_db *db,
    int32_t value);

/**
 * User specified the algorithmic mode
 *
 * @param db the database of user specified algorithmic mode
 *
 * @return void
 */
    void kaps_resource_set_user_specified_algo_mode(
    struct kaps_db *db);

/**
 * Indicates whether the algo mode is specified by user or not
 *
 * @param db the database pointer
 *
 * @return "1" if the user specified the algo mode, otherwise returns "0"
 */
    uint8_t kaps_resource_get_is_user_specified_algo_mode(
    struct kaps_db *db);


/**
 * Indicates the number of free Super Blocks available on the device
 *
 *
 * @param this_device KBP device handle
 *
 * @return number of free Super Blocks available on the device
 */
    uint32_t kaps_resource_get_num_free_sb_on_device(
    struct kaps_device *this_device);

/**
 * Indicates the number of ABs used by the db in this device
 *
 * @param device KBP device handle
 * @param db valid database handle
 *
 * @return number of ABs used by the db in this device
 */
    uint32_t kaps_resource_get_ab_usage(
    struct kaps_device *this_device,
    struct kaps_db *db);

/**
 * Output the AB information to File
 *
 * @param db valid database handle
 * @param fp file pointer to dump the output
 */

    void kaps_resource_print_ab_info(
    struct kaps_db *db,
    FILE * fp);



/**
 *
 * same as above api, checks the searches of all broadcast devices
 *
 * @param db1 valid database handle
 * @param db2 valid database handle
 * @}
 */
    int32_t kaps_resource_is_searched_parallel(
    struct kaps_db *db1,
    struct kaps_db *db2);



/**
 *
 * checks whether common uda manager can be assigned for two XOR DB
 *
 * @param db1 valid database handle
 * @param db2 valid database handle
 * @}
 */
    int32_t kaps_resource_can_share_xor_uda(
    struct kaps_db *db1,
    struct kaps_db *db2);

/**
 *
 * verifies the DBA allocation
 *
 * @param device valid device handle
 * @}
 */
    kaps_status resource_verify_dba_allocation(
    struct kaps_device *device);

/**
 *
 * prints the currently achieved capacity
 * of a DB into the given file handle
 *
 * @param fp the file handle
 * @param db the database handle
 * @}
 */
    void kaps_resource_print_db_capacity(
    FILE * fp,
    struct kaps_db *db);

/**
 * Fit the DBA resources on the device. This
 * function is similar for all devices with
 * minor quirks
 *
 * @param db the database handle
 * @param device the device on which to fit
 * @param num_rqt_ab number of Array Blocks requested
 * @param is_rqt_fulfilled indicates if the function was able to successfully grant the array blocks
 *
 */

    void resource_fit_dba(
    struct kaps_db *db,
    struct kaps_device *device,
    uint32_t num_rqt_ab,
    int32_t alloc_narrow_ab,
    uint32_t force_skip_small_ab,
    int32_t * is_rqt_fulfilled);

/**
* if dynamic allocation is enabled then it
* calls the resource_fit_dba function
*
* @param db the database handle
* @param num_rqt_ab number of Array Blocks requested
*/
    kaps_status resource_fit_dba_dynamic(
    struct kaps_db *db,
    uint32_t num_rqt_ab,
    int32_t alloc_narrow_ab,
    uint32_t force_skip_small_ab);

/**
* if dynamic allocation is enabled then it
* calls kaps_resource_dynamic_ad_alloc_op or
* kaps_resource_dynamic_ad_alloc_12k based on device type

*
* @param ad the AD database handle
* @param sb_bitmap super block bitmap which holds ad allocation info
* @param num_sb_needed number of SBs needed
*/
    kaps_status kaps_resource_dynamic_ad_alloc(
    struct kaps_db *ad,
    uint8_t * sb_bitmap,
    int32_t num_sb_needed);



/**
* calls the dynamic AB allocation function if Ab list contains
* less than the required no of AB
*
* @param db the database handle
* @param num_rqt_ab number of Array Blocks requested
*/
    kaps_status resource_dba_check_and_alloc(
    struct kaps_db *db,
    uint32_t num_rqt_ab,
    int32_t alloc_narrow_ab);

/**
* releases an AB from an SB and DB back to resource manager.
* Also updates the db owner list of the SB
* @param db the database handle
* @param ab AB to be freed
*/
    kaps_status resource_release_ab(
    struct kaps_db *db,
    struct kaps_ab_info *ab);

/**
* releases the index range assigned to ab AB
* back to the DB.
* @param db the database handle
* @param ab AB to be freed
*/
    kaps_status resource_release_index_range(
    struct kaps_db *db,
    struct kaps_ab_info *ab);

/**
* if dynamic_allocation is enabled, it releases the index range and
* ABs back to the resource manager.
* if dynamic allocation is not enabled then it concatenates the ABs in free list
* with the AB list
*
* @param db the database handle
* @param list list of ABs to be freed
*/
    kaps_status resource_dba_check_and_release(
    struct kaps_db *db,
    struct kaps_c_list *list);

/**
 * Resets active bit in software AB structure.
 *
 * @param ab SW AB structure
 *
 * @returns KAPS_OK on success or an error code
 */
    kaps_status resource_check_and_deactivate_ab(
    struct kaps_ab_info *ab);

/**
 * DBA resource fit for OP
 *
 * @param device device handle
 *
 * @returns KAPS_OK on success or an error code
 */

    kaps_status resource_fit_dba_op(
    struct kaps_device *device);

/**
 * DBA resource fit for OP when user manually assigned resources
 *
 * @param device device handle
 * @param db database handle
 * @param dba_blk_array array holding the DBA blocks assigned
 * @param num_ab number of DBA blocks assigned
 *
 * @returns KAPS_OK on success or an error code
 */
    kaps_status resource_fit_dba_user_specified(
    struct kaps_device *device,
    struct kaps_db *db,
    int32_t * dba_blk_array,
    int32_t num_ab);

/**
 * Process BMRs for ACL databases
 *
 * @param device valid device handle
 *
 * @returns KAPS_OK on success or an error code
 */

    kaps_status resource_process_bmr(
    struct kaps_device *device);

/**
 * Return the type of AD this database is using
 *
 * @param db valid database handle
 *
 * @retval the AD type being used
 */

    enum kaps_ad_type kaps_resource_get_ad_type(
    struct kaps_db *db);

/**
 * Set the AD type to be used by the database
 *
 * @param db valid database handle
 * @param ad_type the AD database type
 */

    void kaps_resource_set_ad_type(
    struct kaps_db *db,
    enum kaps_ad_type ad_type);

/**
 * Returns the number of range units assigned to the database
 *
 * @param db valid database handle
 *
 * @retval number of range units assigned
 */

    uint32_t resource_get_range_units(
    struct kaps_db *db);

/**
 * Returns the AB list pointer for the database
 *
 * @param db valid database handle
 *
 * @reval pointer to list of ABs
 */
    struct kaps_c_list *kaps_resource_get_ab_list(
    struct kaps_db *db);

/**
 * Return the start range unit number for database
 *
 * @param db valid database handle
 *
 * @retval start range unit number
 */

    uint32_t resource_get_start_range_unit(
    struct kaps_db *db);

/**
 * Convert an LPM database to an ACL database
 *
 * @param db valid database handle
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status resource_switch_lpm_to_acl(
    struct kaps_db *db);

/**
 * Assign range units to ACL databases as required
 *
 * @param device the device handle
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status resource_assign_range_units(
    struct kaps_device *device);



/**
 * Returns 1 if database is algorithmic else returns 0
 *
 * @param db valid database handle
 *
 * @return 1 if database is algorithmic else returns 0
 */
    uint8_t kaps_db_get_algorithmic(
    const struct kaps_db *db);

/**
 * Performs pre processing on resources before Warmboot Save
 *
 * @param device valid device handle
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_resource_wb_pre_process(
    struct kaps_device *device);


/**
 * Go through the instructions for KAPS and determine which ABs
 * should be assigned to which database if the combinations
 * of instructions allows a valid configuration of ABs to DBs
 *
 * @param device KBP device handle
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_jr1_resource_assign_rpbs(
    struct kaps_device *device);

/**
 * Sets manually specified resources on database
 *
 * @param db database handle
 * @param resource which resource to set
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status resource_db_set_user_specified(
    struct kaps_db *db,
    uint32_t resource_value,
    ...);

/**
 * Cleans up the UDA resources allocated for this DB
 *
 * @param db db handle
 *
 * @return KAPS_OK on success
 */
    kaps_status resource_release_all_resources(
    struct kaps_db *db);






/**
 * Prints to a given file pointer
 *
 * @retval KAPS_OK on success
 */
    int kaps_print_to_file(
    struct kaps_device *device,
    FILE * fp,
    const char *fmt,
    ...);

  




#ifdef __cplusplus
}
#endif
#endif
