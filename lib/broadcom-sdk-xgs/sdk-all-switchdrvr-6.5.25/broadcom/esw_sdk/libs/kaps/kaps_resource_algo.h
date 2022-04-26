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

#ifndef __KAPS_RESOURCE_ALGO_H
#define __KAPS_RESOURCE_ALGO_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include "kaps_device.h"
#include "kaps_hw_limits.h"
#include "kaps_errors.h"
#include "kaps_list.h"
#include "kaps_device_alg.h"
#include "kaps_resource.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct kaps_db;
    struct kaps_instruction;
    struct kaps_device;
    struct kaps_key;
    struct kaps_uda_mgr;
    struct uda_mgr_playback_chunk;

/**
 * @ingroup KAPS_RM
 * @{
 */

/**
* Default value of UDA power budget
*/
#define MAX_UDA_POWER_BUDGET 32
#define MAX_OP_DBA_POWER_BUDGET  64
#define MAX_12K_DBA_POWER_BUDGET 64

/**
 * Maximum LSN width assigned per XOR database
 */
#define MAX_LSN_WIDTH_PER_XOR_DB  (3)
/**
 * Maximum LSN width assigned per non-XOR database
 */
#define MAX_LSN_WIDTH_PER_NON_XOR_DB  (16)

/**
 * Maximum limit to grow LSN size
 */
#define MAX_LSN_WIDTH_FOR_GROW (8)

/**
 * Minimum LSN width assigned per XOR database
 */
#define MIN_LSN_WIDTH_PER_XOR_DB  (1)
/**
 * Minimum LSN width assigned per non-XOR database
 */
#define MIN_LSN_WIDTH_PER_NON_XOR_DB  (2)

/* Minimum power budget for LPM */
#define MIN_DBA_POWER_BUDGET_LPM 2

/**
* Minimum power budget for ACLs.
* It is decided based on number ABs in a super block.
*/
#define MIN_OP_DBA_POWER_BUDGET_ACL  4

#define MIN_12K_DBA_POWER_BUDGET_ACL 8

/**
 * @brief Enumeration for Manually specifying resources
 */
    enum resource_user_specified
    {
        RESOURCE_RPT_BMP,
        RESOURCE_DBA_BMP,
        RESOURCE_UDM_BMP,
        RESOURCE_USB_BMP,
        RESOURCE_NUM_DBA_DT,
        RESOURCE_NUM_SRAM_DT,
        RESOURCE_MAX_LSN_SIZE,

        RESOURCE_INVALID
    };

/**
 * Failed to allocate LSN memory
 */

#define FAILED_FIT_LSN  4

/**
 * Failed to allocate IT memory
 */

#define FAILED_FIT_IT   8

/**
 * Failed to allocate HITBIT memory
 */

#define FAILED_FIT_HITBIT   16

#define FAILED_FIT_UDA   32

/* Define a load (kind of weightage) for a decision tree. *
* Higher the value indicates higher number of RPT entries
* expected for this database /decision tree.  Below definitions indicates
* an lpm db is expected to have 8 times more RPT entry then the
* RPT entries of dt_1 of an acl db.
*/

#define RPT_LOAD_LPM 8
#define RPT_LOAD_ACL_PC_OVERFLOW_0 3
#define RPT_LOAD_ACL_DT_0 3
#define RPT_LOAD_ACL_OTHER_DT 1

/**
 * Hardware resources that will be used by ACL/LPM database.
 * Assigned by resource management
 */

/**
 * callback for processing input files
 */

    typedef kaps_status(
    *resource_process_fn) (
    struct kaps_device * device,
    struct kaps_db * db,
    char *fname);

/**
 * Initializes the memory map for the 12K Algo device
 *
 * @param device valid device handle whose memory map should be initialized
 */
    kaps_status initialize_12k_mem_map(
    struct kaps_device *main_device);

/**
 * Initializes the memory map for the KAPS device
 *
 * @param device valid device handle whose memory map should be initialized
 */
    kaps_status kaps_initialize_kaps_mem_map(
    struct kaps_device *device);



/**
 * Expand the existing region to grow
 * @param mgr valid kaps_uda_mgr handle
 * @param region_id region to compact and grow
 * @param playback_pointers array playback pointers for compaction
 * @param num_chunks size of the above array
 * @return KAPS_OK on success or error code
 */

    kaps_status kaps_resource_expand_uda_mgr_regions(
    struct kaps_uda_mgr *mgr);

/**
 * Expand the existing region to grow
 * @param mgr valid kaps_uda_mgr handle
 * @param region_id region to compact and grow
 * @param playback_pointers array playback pointers for compaction
 * @param num_chunks size of the above array
 * @return KAPS_OK on success or error code
 */
    kaps_status resource_op_expand_uda_mgr_regions(
    struct kaps_uda_mgr *mgr);

/**
 * Expand the existing region to grow
 * @param mgr valid kaps_uda_mgr handle
 * @param region_id region to compact and grow
 * @param playback_pointers array playback pointers for compaction
 * @param num_chunks size of the above array
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_resource_kaps_expand_uda_mgr_regions(
    struct kaps_uda_mgr *mgr);

/**
 * Returns the number of unused UDA bricks that can possibly be assigned to a database in the future
 * @param device  valid device handle
 * @param db valid database handle
 * @return KAPS_OK on success or error code
*/
    uint32_t kaps_resource_get_num_unused_uda_bricks(
    struct kaps_device *device,
    struct kaps_db *db);

/**
 * Returns the number of unused UDA bricks that can possibly be assigned to a database in the future for the OP device type
 * @param device  valid device handle
 * @param db valid database handle
 * @return KAPS_OK on success or error code
*/
    uint32_t kaps_resource_get_num_unused_uda_bricks_op(
    struct kaps_device *device,
    struct kaps_db *db);

/**
 * Assign and fit resources for KAPS
 *
 * @param device valid device handle
 */

    kaps_status kaps_resource_process(
    struct kaps_device *device);

/**
 * Print visual information of resources allocated
 *
 * @param device KBP device handle
 * @param f the file to output to
 */

    void kaps_resource_kaps_print_html(
    struct kaps_device *device,
    FILE * f);





/**
 * Allocate and fit resources dynamically for Optimus Prime.
 * @param device valid device handle
 * @param mem_map valid memory map handle
 * @param db valid db handle
 * @param dt_index decision tree index for which this allocation is done
 * @param p_start_row start row no of the dynamically allocated region will be returned here
 * @param p_start_lpu start LPU no of the dynamically allocated region will be returned here
 * @param p_num_row no of rows in the dynamically allocated region will be returned here
 * @param p_num_lpu no of LPU in the dynamically allocated region will be returned here
 * @return KAPS_OK on success or error code
 */

    kaps_status kaps_resource_kaps_dynamic_uda_alloc(
    struct kaps_device *device,
    struct memory_map *mem_map,
    struct kaps_db *db,
    int8_t dt_index,
    int32_t * p_num_lpu,
    uint8_t udms[]);

/**
 * Releases an allocated UDM to Resource Manager.
 * @param device valid device handle
 * @param mem_map valid memory map handle
 * @param db valid db handle
 * @param dt_index decision tree index for which this allocation is done
 * @param udc_no which UDC it belongs to
 * @param udm_no which UDM to be free
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_resource_release_udm(
    struct kaps_device *device,
    struct memory_map *mem_map,
    struct kaps_db *db,
    int32_t dt_index,
    int32_t udc_no,
    int32_t udm_no);

/**
 * Releases an allocated XOR UDC to Resource Manager.
 * @param device valid device handle
 * @param mem_map valid memory map handle
 * @param db valid db handle
 * @param dt_index decision tree index for which this allocation is done
 * @param udc_no which UDC to be released
 *
 * @return KAPS_OK on success or error code
 */
    kaps_status kaps_resource_release_xor_udc(
    struct kaps_device *device,
    struct memory_map *mem_map,
    struct kaps_db *db,
    int32_t dt_index,
    int32_t udc_no);

/**
 * Function verifies the sizeof of all important data structures specific to LPM to keep track of heap memory
 */
    void kaps_device_lpm_verify_sizeof_data_structures(
    void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif                          /* __RESOURCE_ALGO_H */
