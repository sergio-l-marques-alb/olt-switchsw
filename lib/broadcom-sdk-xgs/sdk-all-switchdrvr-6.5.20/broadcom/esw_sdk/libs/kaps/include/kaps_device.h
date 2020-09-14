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

#ifndef __KAPS_DEVICE_H
#define __KAPS_DEVICE_H

/**
 * @file device.h
 *
 * This module provides the hardware-abstraction layer (HAL)
 * for the user.
 */

#include <stdint.h>
#include <stdio.h>

#include "kaps_errors.h"
#include "kaps_init.h"
#include "kaps_hw_limits.h"

#ifdef __cplusplus
extern "C" {
#endif



/**
 * @addtogroup DEVICE_API
 * @{
 */

/**
 * Opaque KBP device handle.
 */

struct kaps_device;

/**
 * Opaque allocator handle.
 */

struct kaps_allocator;

/**
 * Opaque user handle to the database stored in the KBP.
 */

struct kaps_db;

/**
 * Opaque handle to an entry within a database.
 */

struct kaps_entry;

/**
 * KBP SDK version string.
 */

/* #define KAPS_SDK_VERSION "KBP SDK 1.5.11" */
#define KAPS_SDK_VERSION "KAPS SDK TRUNK"

#define KAPS_ERROR_LIST_VERSION (2)

/*#define BRCM_FIX_ERR_DBG_PRINTS*/


/**
 * Type of databases supported by software.
 */

enum kaps_db_type {
    KAPS_DB_INVALID, /**< Error database type. */
    KAPS_DB_ACL,     /**< Algorithmic and non-algorithmic ACL databases. */
    KAPS_DB_LPM,     /**< Longest-prefix-match databases. */
    KAPS_DB_EM,      /**< Exact-match databases. */
    KAPS_DB_AD,      /**< Associated data databases. */
    KAPS_DB_DMA,     /**< Direct Memory Access databases. */
    KAPS_DB_HB,      /**< Hit Bits databases. */
    KAPS_DB_COUNTER, /**< Search-dependent-Counter databases */
    KAPS_DB_TAP,     /**< TAP databases */
    KAPS_DB_STATS,   /**< Stats databases */
    KAPS_DB_END      /**< This should be the last entry. */
};

/**
 * @brief Hardware resources that can be partitioned
 *
 * The available hardware resources that can be partitioned between databases. The following functions
 * can be invoked with the hardware resources to limit the databases to use
 * the specified resources only:
 *
 * @li kaps_db_set_resource()
 * @li kaps_ad_db_set_resource()
 *
 * can be invoked with the HW resources to limit the databases to use
 * the specified resources only. Optionally, the resource compiler can
 * be used to automatically assign hardware resources to the various databases
 * and functions. For LPM only databases, the database handle must be used to
 * set both UDA and DBA. For all other databases, the
 * database handle is used to set DBA, and the AD Database handle
 * is used to set UDA
 *
 * | RESOURCE | NUMBER OF ARGS | ARGUMENTS |
 * |:---: | :---: | :---: |
 * | KAPS_HW_RESOURCE_DBA | 1 | uint32_t value specifying the number of ABs to use (for LPM, EM, ACL databases). |
 * | KAPS_HW_RESOURCE_UDA | 1 | uint32_t value specifying the UDA in megabits (for algorithmic LPM, EM, ACL databases and for AD databases). |
 * | KAPS_HW_RESOURCE_RANGE_UNITS | 1 | uint32_t value specifying the number of range units to use for MCOR/DIRPE (ACL databases only). |
 */

enum kaps_hw_resource {
    KAPS_HW_RESOURCE_DBA,  /**< The number of ABs to use. */
    KAPS_HW_RESOURCE_UDA,  /**< The UDA in megabits. */
    KAPS_HW_RESOURCE_DBA_CORE,  /**< The number of ABs to use. */
    KAPS_HW_RESOURCE_UDA_CORE,  /**< The UDA in megabits. */
    KAPS_HW_RESOURCE_RANGE_UNITS, /**< The number of range units to use. */
    KAPS_HW_RESOURCE_INVALID      /**< This should be the last entry. */
};



/**
 * @brief The list of error types:
 *
 * used with kaps_device_inject_errors() as input parameter to Inject/Emulate the error/s
 * used with kaps_device_get_errors() as output parameter to get the ESR status
 */


/**
 * Initialize the KBP device. A valid transport layer is required.
 * On initialization, the device is dynamically queried to ascertain
 * its capabilities and the resources available.
 *
 * @param alloc Valid non-NULL allocator handle for portability.
 * @param type The device type ::kaps_device_type.
 * @param flags Flags OR'd together that allow the device to be set into specific modes. See ::kaps_device_flags.
 * @param xpt NULL or valid transport layer handle. Transport-layer functions are used to
 *              establish communication with the device.
 * @param config SerDes and lane-configuration information. Can be NULL to indicate the initialization
 *              has already occurred.
 * @param device Device handle initialized and returned on success. This handle can be used to
 *        address the global device.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_device_init(struct kaps_allocator *alloc, enum kaps_device_type type, uint32_t flags,
                           void *xpt, struct kaps_device_config *config, struct kaps_device **device);

/**
 * Destroys the device handle and frees up resources. The physical
 * device is left undisturbed.
 *
 * @param device The device handle.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_device_destroy(struct kaps_device *device);

/**
 * Locks the device. Once the device is locked, new databases/instructions
 * cannot be created.
 *
 * @param device Initialized pointer to the device.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_device_lock(struct kaps_device *device);

/**
 * Crash Recovery Status
 */
enum kaps_restore_status {
    KAPS_RESTORE_NO_CHANGE,            /* The SDK was restored with no change in config, no pending actions */
    KAPS_RESTORE_CHANGES_COMMITTED,    /* The SDK was restored and the pending actions were committed */
    KAPS_RESTORE_CHANGES_ABORTED       /* The SDK was restored and the pending actions were aborted */
};





/**
 * Properties that can be set on the device.
 */
enum kaps_device_properties {
    KAPS_DEVICE_PROP_DESCRIPTION,                /**< A string description for the device. */
    KAPS_DEVICE_PROP_TOTAL_NUM_BB,               /**< Total number of Bucket Blocks. */
    KAPS_DEVICE_PROP_RESOURCE_FN,                /**< Callback for resource processing. */
    KAPS_DEVICE_PROP_LTR_SM_UPDATE,              /**< LTR Shadow Memory Update. */
    KAPS_DEVICE_PROP_INTERRUPT,                  /**< Enable for supported device interrupts */
    KAPS_DEVICE_PROP_IFSR_THRESHOLD,             /**< Threshold for initiating a soft repair of DBA */
    KAPS_DEVICE_PROP_LAST_PARITY_ERRORS,         /**< Last seen DBA parity error address of specific device [updated by kaps_device_fix_errors] */
    KAPS_DEVICE_PROP_LAST_PARITY_ERRORS_STATUS,  /**< Last seen DBA parity error address' status of specific device
                                                     [updated by kaps_device_fix_errors] for the status code refer to the
                                                     enum parity_row_status above */
    KAPS_DEVICE_PROP_INJECT_PARITY_ERRORS,       /**< Inject parity errors by using debug regs, few bits in both X,Y row are flipped
                                                     during the PIO writes on the DBA */
    KAPS_DEVICE_PROP_NON_REPAIRABLE_ABS,         /**< Once all the redundant rows are used up, AB cannot repaired
                                                     further, this prop will list those ABs on query (DBA) */
    KAPS_DEVICE_PROP_READ_TYPE,                  /**< read type, 0: read from SM,
                                                                1: read from device/model,
                                                                2: read from device and compare against SM data
                                                     Note: in blackhole mode read type cannot be other than 0, will return error */
    KAPS_DEVICE_PROP_DEBUG_LEVEL,                /**< debug level, default 0, non zero value represents the level of debug prints
                                                     Note: must be set before kaps_device_lock() and debug print APIs must be called after
                                                     the kaps_device_lock() as instruction(s), entries might be in inconsistent state */
    KAPS_DEVICE_PROP_INST_LATENCY,               /**< Value between zero/one, two and eight, where the zero(==1) means issue instructions
                                                     that take minimum amount of data bandwidth, and eight means maximum
                                                     update rate, as longer latency control instructions can be issued. */
    KAPS_DEVICE_PRE_CLEAR_ABS,                   /**< Clear the ABs at the time of device init */
    KAPS_DEVICE_PROP_LAST_UDA_PARITY_ERRORS,     /**< Last seen UDA parity error address of specific device [updated by kaps_device_fix_errors] */
    KAPS_DEVICE_ADD_BROADCAST,                   /**< Configures a device as broadcast device */
    KAPS_DEVICE_ADD_SELECTIVE_BROADCAST,         /**< Configures a device as a selective broadcast device */
    KAPS_DEVICE_BROADCAST_AT_XPT,                /**< If this property is set, then broadcast happens at XPT level,
                                                     else it happens at hardware device driver level */
    KAPS_DEVICE_PROP_CRASH_RECOVERY,             /**< Register the NV Read/Write callbacks required to support Crash Recovery */
    KAPS_DEVICE_PROP_DEFER_LTR_WRITES,           /**< Defer LTR writes. use it for ISSU */
    KAPS_DEVICE_PROP_CLR_DEV_ERR_BITS,           /**< Clears only device error/parity bits, else all bits in ESR : default all*/
    KAPS_DEVICE_PROP_DUMP_ON_ASSERT,             /**< Dump software state to a file before asserting */
    KAPS_DEVICE_PROP_CLOCK_RATE,                 /**< OP2 specific, set the device clock rate */
    KAPS_DEVICE_PROP_ADV_UDA_WRITE,              /**< Enable/Disable advanced UDA write. This is O3S specific, for OP it's the default*/
    KAPS_DEVICE_PROP_RETURN_ERROR_ON_ASSERTS,    /**< Return error code on assert hit */
    KAPS_DEVICE_PROP_COUNTER_COMPRESSION,        /**< To enable compression on counters */
    KAPS_DEVICE_PROP_STAT_RECORD_SIZE,           /**< Stats record size */
    KAPS_DEVICE_PROP_TAP_NULL_REC_SIZE,          /**< TAP NULL record size */
    KAPS_DEVICE_PROP_TAP_INGRESS_REC_SIZE,       /**< TAP Ingress record size */
    KAPS_DEVICE_PROP_TAP_EGRESS_REC_SIZE,        /**< TAP Egress record size */
    KAPS_DEVICE_PROP_TAP_INGRESS_OPCODE_EXT_START_LOC, /**< Start location of Opcode Extension field for all ingress records */
    KAPS_DEVICE_PROP_TAP_EGRESS_OPCODE_EXT_START_LOC, /**< Start location of Opcode Extension field for all egress records */
    KAPS_DEVICE_PROP_TAP_INGRESS_OPCODE_EXT_LEN, /**< Number of valid bits for opcode extension field all ingress records */
    KAPS_DEVICE_PROP_TAP_EGRESS_OPCODE_EXT_LEN,  /**< Number of valid bits for opcode extension field all egress records */
    KAPS_DEVICE_PROP_MIN_RESOURCE,               /**< To enable min resources (low density) module usage */
    KAPS_DEVICE_PROP_FORCE_INVALIDATE_LOC,       /**< Forcibly invalidate the parity location belonging to magic db
                                                     (reserved/used by external resource) [used by only kaps_device_fix_errors]*/
    KAPS_DEVICE_PROP_HANDLE_INTERFACE_ERRORS,    /**< 0: Disable interface error handling functionality
                                                     in SDK (in case application handling itself)  [default its turn on] */
    KAPS_DEVICE_PROP_INVALID                     /**< This should be the last entry. */
};

/**
 * @cond INTERNAL
 *
 * @file device_kaps.h
 *
 * Defines depths, widths and the number of for all software-visible resources in KAPS
 */


#define KAPS_AD_WIDTH_1 (20)

#define KAPS_SEARCH_INTERFACE_WIDTH_1 (160)

#define KAPS_SEARCH_INTERFACE_WIDTH_8 (20)

#define KAPS_DBA_WIDTH_8 (20)

#define KAPS_DBA_WIDTH_1 (160)

#define KAPS_REGISTER_WIDTH_1 (32)

#define KAPS_REGISTER_WIDTH_8 (4)

#define KAPS_HB_ROW_WIDTH_8 (2)

#define KAPS_HB_ROW_WIDTH_1 (16)





/*Device Ids*/

#define KAPS_DEFAULT_DEVICE_ID (0x0001)

#define KAPS_QUMRAN_DEVICE_ID (0x0002) /*QAX*/

#define KAPS_JERICHO_PLUS_DEVICE_ID (0x0003)

#define KAPS_QUX_DEVICE_ID  (0x0004)

#define KAPS_JERICHO_2_DEVICE_ID      (0x0005)



/*JR+ sub type*/
#define KAPS_JERICHO_PLUS_SUB_TYPE_FM0 (0)

#define KAPS_JERICHO_PLUS_SUB_TYPE_FM4 (1)

#define KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM0 (2)

#define KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4 (3)


/*JR2 sub types*/
#define KAPS_JERICHO_2_SUB_TYPE_TWO_LEVEL (0)

#define KAPS_JERICHO_2_SUB_TYPE_THREE_LEVEL (1)

#define KAPS_JERICHO_2_SUB_TYPE_Q2A_THREE_LEVEL (2)

#define KAPS_JERICHO_2_SUB_TYPE_J2P_THREE_LEVEL (3)




/*Revision register*/

#define KAPS_REVISION_REGISTER_ADDR (0x0)

#define KAPS_JERICHO_REVISION_REG_VALUE (0x00010000)

#define KAPS_QUMRAN_AX_REVISION_REG_VALUE (0x00020000)

#define KAPS_JERICHO_PLUS_FM0_REVISION_REG_VALUE (0x00030000)

#define KAPS_JERICHO_PLUS_FM4_REVISION_REG_VALUE (0x00030001)

#define KAPS_JERICHO_PLUS_JER_MODE_FM0_REVISION_REG_VALUE (0x00030002)

#define KAPS_JERICHO_PLUS_JER_MODE_FM4_REVISION_REG_VALUE (0x00030003)


#define KAPS_QUX_REVISION_REG_VALUE (0x00040000)

#define KAPS_JERICHO_2_REVISION_REG_VALUE (0x00050000)








/**
 * @brief KAPS DBA entry
 */

struct kaps_dba_entry
{
    uint8_t key[KAPS_SEARCH_INTERFACE_WIDTH_8]; /**< 160b data or mask */
    uint32_t resv:4;       /**< Unused */
    uint32_t is_valid:2;   /**< Valid bit per 80b part of data/mask */
    uint32_t pad:2;       /**< SW pad */
};

/**
 * @brief KAPS AD info representation
 */
struct kaps_ad_entry
{

    uint32_t ad:20;   /**< Associated data */
    uint32_t resv:12; /**< unused */
    uint32_t resv1;/**< unused */
    uint32_t resv2;/**< unused */
    uint32_t resv3;/**< unused */
};


/**
 * Sets a specific property for the device..
 *
 * @param device Valid device handle
 * @param property Device properties ::kaps_device_properties.
 * @param ... Variable arguments based on property.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_device_set_property(struct kaps_device *device, enum kaps_device_properties property, ...);

/**
 * Gets a specific property of the device.
 *
 * @param device Valid device handle.
 * @param property Device properties being queried ::kaps_device_properties.
 * @param ... Variable arguments based on property.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_device_get_property(struct kaps_device *device, enum kaps_device_properties property, ...);

/**
 * Obtains the SMT thread device handle. The device must have been initialized in SMT
 * mode. Databases and instructions must be created using the appropriate thread's device
 * handle.
 *
 * @param device Valid device handle returned by kaps_device_init().
 * @param tid The SMT thread ID. Currently supported IDs are zero and one.
 * @param thr_device The thread's device handle returned on success.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_device_thread_init(struct kaps_device *device, uint32_t tid, struct kaps_device **thr_device);

/**
 * Prints the device information. A visual HTML format with SVG
 * is dumped that works well on Firefox and Chrome browsers only.
 *
 * This function should be called only after the device is locked.
 * Calling on an unlocked device will result in an error being
 * returned and no HTML being generated.
 *
 * @param device Valid KBP device handle.
 * @param fp The file pointer to dump to.
 *
 * @retval KAPS_OK On success.
 * @retval KAPS_DEVICE_UNLOCKED when called on a device which has not been locked.
 */

kaps_status kaps_device_print_html(struct kaps_device *device, FILE *fp);


/**
 * Prints the current sw state
 *
 * This function should be called only after the device is locked.
 * Calling on an unlocked device will result in an error being
 * returned and no HTML being generated.
 *
 * @param device Valid KBP device handle.
 * @param fp The file pointer to dump to.
 *
 * @retval KAPS_OK On success.
 * @retval KAPS_DEVICE_UNLOCKED when called on a device which has not been locked.
 */
kaps_status kaps_device_print_sw_state(struct kaps_device * device, FILE * fp);


/**
 * Prints device information in text to the specified file.
 *
 * @param device Valid KBP device handle.
 * @param fp The file pointer to dump to.
 *
 * @retval KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_device_print(struct kaps_device *device, FILE *fp);

/**
 * Obtains the data present in a KAPS block from software and not from hardware
 * This API is used for correcting errors
 *
 * @param device Valid KAPS device handle.
 * @param blk_nr the block for which we need to get the data.
 * @param row_nr row number in the block for which we need to get the data
 * @param func_num indicates the type of KAPS operation which is performed on the block
 * @param nbytes number of bytes present in the array bytes.
 *              If the num bytes is insufficient, then an assert is issued
 * @param bytes the array in which the data in the block is passed back
 *
 * @retval KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_device_get_ser_data(struct kaps_device *device, uint32_t blk_nr,
            uint32_t row_nr, uint32_t func_num, uint32_t nbytes, uint8_t *bytes);

/**
 * Returns the KAPS SDK version string which should not be modified and freed
 *
 *
 * @retval KAPS SDK version string
 */

const char * kaps_device_get_sdk_version(void);






/**
 * @}
 */

/**
 * @addtogroup ISSU_API
 * @{
 */

/**
 * Callback function to read saved state from nonvolatile memory.
 * The user implements this function as specified in the prototype.
 *
 * @param handle Handle passed to API kaps_device_save_state()/ kaps_device_restore_state()
 * @param buffer Valid buffer memory in which to read the data. Only the specified size must be copied, or a buffer overrun can result.
 * @param size The number of bytes to read.
 * @param offset The offset in nonvolatile memory to read from.
 *
 * @retval 0 On success and contents of buffer are valid.
 * @retval Nonzero on failure reading data; buffer contains garbage.
 */
typedef int32_t (*kaps_device_issu_read_fn) (void *handle, uint8_t *buffer, uint32_t size, uint32_t offset);

/**
 * Callback function to write state into nonvolatile memory.
 * The user implements this function as specified in the prototype.
 *
 * @param handle Handle passed to API kaps_device_save_state()/ kaps_device_restore_state()
 * @param buffer Valid buffer memory to take the data from and write into nonvolatile memory.
 * @param size The number of bytes to write.
 * @param offset The offset in the nonvolatile memory to write to.
 *
 * @retval 0 On success and contents of buffer were written out.
 * @retval Nonzero on failure writing data. This result is fatal.
 */
typedef int32_t (*kaps_device_issu_write_fn) (void *handle, uint8_t *buffer, uint32_t size, uint32_t offset);

/**
 * Perform a warm boot/ISSU. Save device state to nonvolatile memory. The
 * user is expected to provide implementations of read/write callbacks.
 * These callbacks are used to write out state into the nonvolatile
 * memory. Once the function is invoked, no further control-plane
 * operations are permitted. API kaps_device_destroy() can be called afterwards
 * to recover control-plane heap resources.
 *
 * @param device Valid device handle.
 * @param read_fn Callback to read data from nonvolatile memory.
 * @param write_fn Callback to write data to nonvolatile memory.
 * @param handle User handle which will be passed back through read_fn and write_fn.
 *
 * @return KAPS_OK on success, or an error code otherwise.
 */

kaps_status kaps_device_save_state(struct kaps_device *device, kaps_device_issu_read_fn read_fn, kaps_device_issu_write_fn write_fn, void *handle);

/**
 * Perform a warm boot/ISSU. Save device state to nonvolatile memory. The
 * user is expected to provide implementations of read/write callbacks.
 * These callbacks are used to write out state into the nonvolatile
 * memory. Control plane operations are permitted after invoking this API.
 *
 * @param device Valid device handle.
 * @param read_fn Callback to read data from nonvolatile memory.
 * @param write_fn Callback to write data to nonvolatile memory.
 * @param handle User handle which will be passed back through read_fn and write_fn.
 *
 * @return KAPS_OK on success, or an error code otherwise.
 */

kaps_status kaps_device_save_state_and_continue(struct kaps_device *device, kaps_device_issu_read_fn read_fn, kaps_device_issu_write_fn write_fn, void *handle);


/**
 * Perform a warm boot/ISSU restore. Restore device state from nonvolatile memory. The
 * user is expected to provide implementations of read/write callbacks.
 * These callbacks are used to read state from the nonvolatile
 * memory. The user is expected to know where the data was stored during
 * the save operation; the offsets specified in the callback are relative to this.
 *
 * @param device Valid device handle.
 * @param read_fn Callback to read data from nonvolatile memory.
 * @param write_fn Callback to write data to nonvolatile memory.
 * @param handle User handle which will be passed back through read_fn and write_fn
 *
 * @return KAPS_OK on success, or an error code otherwise.
 */

kaps_status kaps_device_restore_state(struct kaps_device *device, kaps_device_issu_read_fn read_fn, kaps_device_issu_write_fn write_fn, void *handle);

/**
 * Signal start of reconciliation phase for ISSU. This is an
 * optional phase that signals marking all entries alive
 * in the user's software as visited. The window is demarked
 * by kaps_device_reconcile_start() and kaps_device_reconcile_end().
 * Also see kaps_entry_set_used().
 *
 * @param device Valid device handle.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
kaps_status kaps_device_reconcile_start(struct kaps_device *device);

/**
 * Signal end of reconciliation phase for ISSU. This is an
 * optional phase that signals marking all entries alive
 * in the users software as visited. The window is demarked
 * by kaps_device_reconcile_start() and kaps_device_reconcile_end().
 * Also see kaps_entry_set_used().
 *
 * @param device Valid device handle.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
kaps_status kaps_device_reconcile_end(struct kaps_device *device);

/**
 * @}
 */

/**
 * This is debug API
 * Given hardware index, will do look up and give the DB and Entry handle for ACLs
 * Only DB handle is returned back for LPM databases.
 *
 * @param device Valid KBP device handle.
 * @param hw_index The device index for with the handles are returned, for cascade devices, hw_index must include device_id.
 * @param db The database handle returned if hw_index lookup found, else NULL.
 * @param entry The entry handle returned if hw_index lookup and db found, else NULL.
 *   user can still use the db handle if it is not NULL
 *
 * @retval KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_device_lookup_index(struct kaps_device *device, uint32_t hw_index, struct kaps_db **db, struct kaps_entry **entry);

/**
 * Dumps the current state of the s/w (Key, Database properties, Entries,
 * Instructions, Resources) in the XML Format.
 *
 * @param device Valid KBP device handle
 * @param dump_xml_data_with_entries will the print the entries in the given XML file
 * @param fp Valid XML file pointer to dump the contents to
 *
 */

kaps_status kaps_device_dump(struct kaps_device * device, uint32_t dump_xml_data_with_entries, FILE *fp);

/**
 * @addtogroup CRASH_RECOVERY_API
 * @{
 */

/**
 * This API call marks the start of the transaction in the code flow when Crash Recovery is enabled.
 *
 * @param device Valid device handle.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_device_start_transaction(struct kaps_device *device);

/**
 * This API call marks the end of the transaction in the code flow when Crash Recovery is enabled.
 *
 * @param device Valid device handle.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

kaps_status kaps_device_end_transaction(struct kaps_device *device);

/**
 * This API returns the action taken on the pending entries during crash recovery.
 *
 * @param device Valid device handle.
 * @param status Status of pending entries
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
kaps_status kaps_device_query_restore_status(struct kaps_device *device, enum kaps_restore_status *status);

/**
 * This API clears the status of pending entries being maintained.
 *
 * @param device Valid device handle.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
kaps_status kaps_device_clear_restore_status(struct kaps_device *device);

/**
 * @}
 */





/**
 * This API will return the last error of the device
 *
 * @param device Valid device handle.
 *
 * @return verbose_error_string if any error or empty string if no error. DO NOT FREE THE RETURN STRING
 */

const char *kaps_device_get_last_error(struct kaps_device *device);

/**
 * @endcond
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /*__DEVICE_H */

