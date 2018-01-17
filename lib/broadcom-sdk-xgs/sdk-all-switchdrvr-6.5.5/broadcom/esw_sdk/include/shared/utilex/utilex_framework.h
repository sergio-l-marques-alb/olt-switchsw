/* $Id: sand_framework.h,v 1.13 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 * $
*/
/*! \file utilex_framework.h 
  *  
  */

#ifndef UTILEX_FRAMEWORK_H_INCLUDED
/* { */
#define UTILEX_FRAMEWORK_H_INCLUDED

/*************
* INCLUDES  *
*************/

#include <sal/types.h>

#if 0
/* 
 * { 
 */
#include <sal/limits.h>
#include <sal/core/libc.h>
#include <sal/compiler.h>
/* 
 * }
 */
#endif
#include <shared/shrextend/shrextend_debug.h>

/* $Id: sand_framework.h,v 1.13 Broadcom SDK $
 *  Print and debug possibilities:
 */

/*
 *  The debug and printing features are disabled. 
 *  If this level is selected, the relevant functions,
 *  such as enum-to-string conversion functions,
 *  printing functions etc., are compiled - out.
 *  This level is recommended only for systems without printing
 *  capabilities
 */
/*
 *  Allow printing structures and converting enumerators to strings.
 *  The enum-to-string and printing functions are compiled in
 *  (the library size is affected accordingly).
 *  Unless explicitly requested by calling the relevant function 
 *  e.g. string conversion or printing, does not have any run-time impact.
 *  The driver does not initiate printing (error messages or any other).
 *  The printing is available if called explicitly.
 *  Recommended for systems in production, if TCM is used for
 *  error messaging, and the extra-information available 
 *  when setting higher printing/debug levels is not required
 */
#define UTILEX_DBG_LVL1 1
  /*
   *  In addition to the functionality supplied by UTILEX_DBG_LVL1,
   *  implicitly prints additional information upon error (only):
   *  1. Prints the values of the input parameters of APIs - i.e., 
   *  parameters with a type 'SOC_SAND_IN' (for new devices and driver versions).
   *  2. For some error conditions, calls a relevant diagnostics function
   *  and prints the output of this function.
   *  Recommended for systems in bring-up stage, if printing functionality
   *  is supported.
   *  Imposes additional increase of the library size
   */
#define UTILEX_DBG_LVL2 2
  /*
   *  In addition to the functionality supplied by UTILEX_DBG_LVL2,
   *  the driver can print debug information, even if not under an error condition. 
   *  (log-printing mode).
   */
#define UTILEX_DBG_LVL3 3

  /*
   *    Recommended for systems in bring-up phase.
   *  refer to the documentation on printing levels for 
   *  a more detailed explanation
   */
#define UTILEX_DEBUG UTILEX_DBG_LVL2

/*
 * When set, the driver will print error messages, in the driver context.
 * For a production-stage system, this flag is typically unset ('0'),
 * and the error printing is done by the time callback module (TCM),
 * after getting a message from the driver, through a message queue.
 */
#define UTILEX_ALLOW_DRIVER_TO_PRINT_ERRORS (UTILEX_DEBUG >= UTILEX_DBG_LVL2)


/*
 * NULL pointer
 */
#ifndef NULL
#define NULL (void *)0
#endif
/*
 * boolean true
 */
#ifndef TRUE
#define TRUE 1
#endif
/*
 * boolean false
 */
#ifndef FALSE
#define FALSE 0
#endif

/*
 * basic types definition
 */

#define UTILEX_BOOL2NUM(b_val) ((b_val) == FALSE?0x0:0x1)
#define UTILEX_NUM2BOOL(n_val) ((uint8)((n_val) == 0x0?FALSE:TRUE))

/*
 *  Invert the result - TRUE if value is 0, FALSE otherwise
 */
#define UTILEX_BOOL2NUM_INVERSE(b_val) ((b_val) == TRUE?0x0:0x1)
#define UTILEX_NUM2BOOL_INVERSE(n_val) (uint8)((n_val) == 0x0?TRUE:FALSE)

/*
 *  Range-related, unsigned.
 *  Get count of entities, first and last entity in range.
 */
#define UTILEX_RNG_COUNT(n_first, n_last) (((n_last) >= (n_first))?((n_last) - (n_first) + 1):((n_first) - (n_last) + 1))
#define UTILEX_RNG_FIRST(n_last, n_cnt)   ((((n_last) + 1) >= (n_cnt))?(((n_last) + 1) - (n_cnt)):0)
#define UTILEX_RNG_LAST(n_first, n_cnt)   ((n_first) + (n_cnt) - 1)
#define UTILEX_RNG_NEXT(n_curr)           ((n_curr) + 1)
#define UTILEX_RNG_PREV(n_curr)           (((n_curr) > 0)?((n_curr) - 1):0)

/*
 * basic return type of basic driver methods
 */
typedef unsigned short UTILEX_RET;

/*
 * the basic UTILEX_RET for error
 */
#define UTILEX_ERR 1
/*
 * the basic UTILEX_RET for success
 */
#define UTILEX_OK  0
/*
 * all input params to a method must be one of these 3
 */
#define UTILEX_IN   const
#define UTILEX_OUT
#define UTILEX_INOUT

#define UTILEX_NOF_BITS_IN_BYTE 8

#define UTILEX_BIT_BYTE_SHIFT 3

#define UTILEX_TRANSLATE_BITS_TO_BYTES(_var)   \
           ((_var) >> UTILEX_BIT_BYTE_SHIFT)

/*
 * A generic indication for an invalid value, 32-bit.
 * May be used by internal driver functions
 */
#define UTILEX_INTERN_VAL_INVALID_32  0xffffffff

/*
 *	A value marking invalid register in the device
 */
#define UTILEX_REG_VAL_INVALID   0xDEADBEAF

/* 
 * TRUE if the internal value is the "invalid-value" indication
 */
#define UTILEX_IS_INTERN_VAL_INVALID(intern_val)  \
  UTILEX_NUM2BOOL(intern_val == UTILEX_INTERN_VAL_INVALID_32)

/*
 * Byte swapping MACRO.
 */
#define UTILEX_BYTE_SWAP(x) ((((x) << 24)) | (((x) & 0xff00) << 8) | (((x) & 0xff0000) >> 8) | (((x) >> 24)))

/*
 * read / write from the chip must take one of this forms
 */
typedef enum
{
  UTILEX_DIRECT,
  UTILEX_INDIRECT
} UTILEX_ACCESS_METHOD;
/*
 * sets the x bit in a word, and only it
 */
#ifndef UTILEX_BIT
#define UTILEX_BIT(x) (1UL<<(x))
#endif
/*
 * resets the x bit in a word, and only it
 */
#ifndef UTILEX_RBIT
#define UTILEX_RBIT(x) (~(1UL<<(x)))
#endif

#if 0
/* 
 * { 
 */
/*
 * The number of devices this infrastructure
 * is willing to manage.
 * User which system supports more than 8 devices
 * per CPU should change this parameter.
 */
#ifndef SOC_SAND_MAX_DEVICE
#define SOC_SAND_MAX_DEVICE SOC_MAX_NUM_DEVICES
#endif

/* } */
#endif

/*
 */
#define UTILEX_OFFSETOF(x,y)  ((uint32)((char*)(&(((x *)0)->y)) - (char*)0))
/*
 * If 'true' then 'val_max' is meaningful.
 */
/*
 * If 'true' then 'val_min' is meaningful.
 */
/*
 */

typedef enum
{
  UTILEX_PRINT_FLAVORS_SHORT = 0,
  UTILEX_PRINT_FLAVORS_NO_ZEROS,
  UTILEX_PRINT_FLAVORS_ERRS_ONLY
} UTILEX_PRINT_FLAVORS;

/*
 * General peruse OPERATION enumerator.
 */
typedef enum
{
  /*
   * No operation indicator.
   */
  UTILEX_NOP = 0,

  /*
   * 'AND' indicator.
   */
  UTILEX_OP_AND,

  /*
   * 'OR' indicator.
   */
  UTILEX_OP_OR,

  /*
   * Last one.
   * Count the number of possible operations.
   */
  UTILEX_NOF_SAND_OP
} UTILEX_OP;

typedef enum
{
  /*
   *  Operation ended with success                            
   */
  UTILEX_SUCCESS = 0,
  /*
   *  Operation failed due to lack of resources, e.g., there 
   *  is no space in the Exact match that map the key to the 
   *  table index or data                                     
   */
  UTILEX_FAILURE_OUT_OF_RESOURCES = 1,
  /*
   *  Operation failed due to lack of resources.When there are 
   *  two resources, this indicates that the second resource 
   *  type is missing                                         
   */
  UTILEX_FAILURE_OUT_OF_RESOURCES_2 = 2,
  /*
   *  Operation failed due to lack of resources.When there are 
   *  three resource types, this indicates that the third 
   *  resource type is missing                                
   */
  UTILEX_FAILURE_OUT_OF_RESOURCES_3 = 3,
  /*
   *  Entries in some databases cannot be updated on the fly. 
   *  This value indicates that the item under configuration 
   *  was already added, and the user is expected to remove it 
   *  before using the table index or key                     
   */
  UTILEX_FAILURE_REMOVE_ENTRY_FIRST = 10,
  /*
   *  Operation failed due to an internal error.                    
   */
  UTILEX_FAILURE_INTERNAL_ERR = 12,
  /*
   *    Operation failed, unspecified error cause. 
   */
  UTILEX_FAILURE_UNKNOWN_ERR = 100
} UTILEX_SUCCESS_FAILURE;

/*
 *  Number of types in UTILEX_SUCCESS_FAILURE
 */
#define UTILEX_NOF_SUCCESS_FAILURES (6)

/*
 *	Converts from UTILEX_SUCCESS_FAILURE
 *  to uint8, TRUE if SUCCESS
 */
#define UTILEX_SUCCESS2BOOL(success_status_) \
  (((success_status_) == UTILEX_SUCCESS)?TRUE:FALSE)
/*
 *	Converts from UTILEX_SUCCESS_FAILURE
 *  to uint8, TRUE if SUCCESS
 */
#define UTILEX_BOOL2SUCCESS(is_success_) \
  (((is_success_) == TRUE)?UTILEX_SUCCESS:UTILEX_FAILURE_UNKNOWN_ERR)

typedef enum
{
  /*
   *  Don't Perform any update/change
   */
  UTILEX_OPERATION_TYPE_NONE = 0,
  /*
   *  Add Operation
   */
  UTILEX_OPERATION_TYPE_ADD = 1,
  /*
   *  Remove Operation
   */
  UTILEX_OPERATION_TYPE_REMOVE = 2,
  /*
   *  Update Operation
   */
  UTILEX_OPERATION_TYPE_UPDATE = 3,
  /*
   *  Number of types in UTILEX_SUCCESS_FAILURE
   */
  UTILEX_NOF_OPERATION_TYPES = 4
} UTILEX_OPERATION_TYPE;

/*
 * Macros related to handling of SOC_SAND registers.
 */

#define UTILEX_REG_MAX_BIT    31
#define UTILEX_REG_SIZE_BITS  32
#define UTILEX_REG_SIZE_BYTES 4

/*
 * Take value and put it in its proper location within a 'long'
 * register (and make sure it does not effect bits outside its
 * predefined mask).
 */
#define  UTILEX_SET_FLD_IN_PLACE(val,shift,mask) (uint32)(((uint32)(val) << ((uint32)shift)) & (uint32)(mask))
/*
 * Get a value out of location within a 'long' register (and make sure it
 * is not effected by bits outside its predefined mask).
 */
#define  UTILEX_GET_FLD_FROM_PLACE(val,shift,mask) (((uint32)(val) & (mask)) >> (shift))

#define  UTILEX_BITS_MASK(ms_bit,ls_bit) \
    ( ((uint32)SAL_BIT(ms_bit)) - ((uint32)SAL_BIT(ls_bit)) + ((uint32)SAL_BIT(ms_bit)) )

#define  UTILEX_ZERO_BITS_MASK(ms_bit,ls_bit)  (~(UTILEX_BITS_MASK(ms_bit, ls_bit)))

#define  UTILEX_RBITS_MASK(ms_bit,ls_bit)      (UTILEX_ZERO_BITS_MASK(ms_bit, ls_bit))

#define  UTILEX_SET_BITS_RANGE(val,ms_bit,ls_bit) \
 (((uint32)(val) << (ls_bit)) & (UTILEX_BITS_MASK(ms_bit,ls_bit)))

#define  UTILEX_GET_BITS_RANGE(val,ms_bit,ls_bit) \
 (((uint32)(val) & (UTILEX_BITS_MASK(ms_bit,ls_bit))) >> (ls_bit))

#define  UTILEX_GET_BIT(val,bit) (UTILEX_GET_BITS_RANGE(val,bit,bit)?(uint32)0x1:(uint32)0x0)

/*
 * Set/Get of bytes.
 * Byte-0: Bits 24-31
 * Byte-1: Bits 16-23
 * Byte-2: Bits 8 -15
 * Byte-3: Bits 0 -7
 */
#define  UTILEX_GET_BYTE_3(val)  UTILEX_GET_FLD_FROM_PLACE(val,0,  0x000000FF)
#define  UTILEX_GET_BYTE_2(val)  UTILEX_GET_FLD_FROM_PLACE(val,8,  0x0000FF00)
#define  UTILEX_GET_BYTE_1(val)  UTILEX_GET_FLD_FROM_PLACE(val,16, 0x00FF0000)
#define  UTILEX_GET_BYTE_0(val)  UTILEX_GET_FLD_FROM_PLACE(val,24, 0xFF000000)

#define  UTILEX_SET_BYTE_2(val)  UTILEX_SET_FLD_IN_PLACE(val,8,  0x0000FF00)
#define  UTILEX_SET_BYTE_1(val)  UTILEX_SET_FLD_IN_PLACE(val,16, 0x00FF0000)

#if 0
/* { */

#define SOC_SAND_SET_BIT(reg,val,bit_i)         \
  reg = (val ? (reg | SOC_SAND_BIT(bit_i)) : (reg & SOC_SAND_RBIT(bit_i)));

#define SOC_SAND_APPLY_MASK(_reg, _enable, _mask)   \
    _reg = (_enable ? (_reg | _mask) : (_reg & ~_mask));

/*
 * Basic SOC_SAND types limits
 */
#define SOC_SAND_U8_MAX 0xff

#define SOC_SAND_U16_MAX 0xffff
#define SOC_SAND_I16_MAX 0x7fff

#define SOC_SAND_U32_MAX 0xffffffff

#define SOC_SAND_UINT_MAX SAL_UINT32_MAX

/*
 * Macro to remove all the compiler
 * warnings on unreferred variables
 */
#define SOC_SAND_IGNORE_UNUSED_VAR(p)     (void)(p)

/*
 *  Infinite loop
 */
#define SOC_SAND_LOOP_FOREVER for(;;)

/*
 * Minimal time between two activations of TCM task, in system
 * ticks. This limits the CPU load due to TCM task and, accordingly,
 * the precision of time periods requested by the user in commands
 * involved with tcm, and interrupt handling latency.
 */
#define SOC_SAND_MIN_TCM_ACTIVATION_PERIOD  1
/*
 * This definition is only related to indirect access.
 * Maximal time between asserting the 'trigger' bit and
 * getting response from device, in nanoseconds.
 */
#define SOC_SAND_TRIGGER_TIMEOUT            1000
/*
 * This definition is only related to DRAM init access.
 * Maximal time between asserting the 'trigger' bit and
 * getting response from device, in nanoseconds.
 */
/*
 * This definition is only related to sending fabric cells.
 * Maximal time between asserting the transmit 'trigger'
 * bit and getting response from device (transmit 'trigger'
 * bit deasserted), in nanoseconds.
 */
/*
 * Size of buffer handed over to driver, to put results in,
 * for callback procedure. Driver may use part or all
 * buffer to store results in.
 */
#define SOC_SAND_CALLBACK_BUF_SIZE          500
/*
 * Pointer to error handler procedure which may be
 * called when driver detects an error condition.
 * Error handler returns int and gets, as parameters,
 * uint32, char *, char **,
 *     uint32,uint32,uint32,
 *     uint32,uint32,uint32
 *
 *
 *    uint32 err_id -
 *      Identifier of error. See ERROR RETURN VALUES
 *      in soc_sand_error_code.h
 *    char          *err_desc -
 *      Null terminated string describing error.
 *      This is the buffer the user gave last time the
 *      function was called.
 *    char          **new_buf -
 *      New buffer to be used next time this function
 *      is to be called.
 *      Case NULL is loaded, the buffer is not replaced.
 *    uint32 param_01 -
 *      General parameters related to error description.
 *    uint32 param_02 -
 *      General parameters related to error description.
 *    uint32 param_03 -
 *      General parameters related to error description.
 *    uint32 param_04 -
 *      General parameters related to error description.
 *    uint32 param_05 -
 *      General parameters related to error description.
 *    uint32 param_06 -
 *      General parameters related to error description.
 */
typedef int (
  *SOC_SAND_ERROR_HANDLER_PTR) (
  uint32,
  char *,
  char **,
  uint32,
  uint32,
  uint32,
  uint32,
  uint32,
  uint32);
/*
 * local error registering and handling
 * {
 */
typedef struct
{
  uint32 err_id;
  char error_txt[SOC_SAND_CALLBACK_BUF_SIZE];
  uint32 param_01;
  uint32 param_02;
  uint32 param_03;
  uint32 param_04;
  uint32 param_05;
  uint32 param_06;
} SOC_SAND_ERRORS_QUEUE_MESSAGE;

typedef struct
{
  /*
   *  Start of the range.                                     
   */
  uint32 start;
  /*
   *  End of the range [start, end]: May be closed or open 
   *  range according to what is specified for the specific 
   *  use.                                                    
   */
  uint32 end;

} SOC_SAND_U32_RANGE;

#define SOC_SAND_TBL_ITER_SCAN_ALL        (0xFFFFFFFF)
#define SOC_SAND_TBL_ITER_SET_BEGIN(iter) ((*(iter)) = 0)
#define SOC_SAND_TBL_ITER_SET_END(iter) ((*(iter)) = 0xFFFFFFFF)
#define SOC_SAND_TBL_ITER_IS_BEGIN(iter) (*(iter) == 0)
#define SOC_SAND_TBL_ITER_IS_END(iter) (*(iter) == 0xFFFFFFFF)

typedef struct
{
  /*
   *  Iterator indicates the place to start to act from 
   *  (read/delete/modify).                                   
   */
  uint32 iter;
  /*
   *  The number of entries to scan.Stop after scanning this 
   *  number of entries. set to SOC_SAND_TBL_ITER_SCAN_ALL to scan all entries
   */
  uint32 entries_to_scan;
  /*
   *  Number of entries to act on.Stop after acting on this 
   *  number of entries.                                      
   */
  uint32 entries_to_act;

} SOC_SAND_TABLE_BLOCK_RANGE;

/*
 *  Maximal number of messages in 'errors' queue
 *  which contains information concerning system
 *  wide events/errors and which is handled by
 *  TCM task. TCM task sends those reports to a
 *  user-supplied callback for system-wide
 *  error handling.
 */
#define ERRORS_MSG_QUEUE_NUM_MSGS       30
/*
 *  Number of system ticks to wait for 'errors' queue
 *  which contains information concerning system
 *  wide events/errors and which is handled by
 *  TCM task.
 */
/*
 * Size of one message in 'errors' queue
 */
#define SOC_SAND_ERRORS_MSG_QUEUE_SIZE_MSG      sizeof(SOC_SAND_ERRORS_QUEUE_MESSAGE)

extern uint32 Soc_sand_errors_msg_queue_flagged;
extern char *Soc_sand_supplied_error_buffer;
extern uint32 Soc_sand_supplied_error_handler_is_on;

SOC_SAND_RET soc_sand_error_handler(
  uint32 err_id,
  const char *error_txt,
  uint32 param_01,
  uint32 param_02,
  uint32 param_03,
  uint32 param_04,
  uint32 param_05,
  uint32 param_06);

SOC_SAND_RET soc_sand_invoke_user_error_handler(
  uint32 err_id,
  const char *error_txt,
  uint32 param_01,
  uint32 param_02,
  uint32 param_03,
  uint32 param_04,
  uint32 param_05,
  uint32 param_06);

/*
 */
SOC_SAND_RET soc_sand_set_user_error_handler(
  SOC_SAND_ERROR_HANDLER_PTR user_error_handler,
  char *user_error_buffer);

SOC_SAND_RET soc_sand_set_user_error_state(
  uint32 onFlag);
/*
 * }
 * local error registering and handling
 */

/*
 * return TRUE / FALSE
 */
#define SOC_SAND_UINT32_ALIGN_MASK (SOC_SAND_BIT(0) | SOC_SAND_BIT(1))

/* allocate memory dynamically for a given type 'DS_ELEM_TYPE' of a data structure
 *NOTE: DS_ELEM_TYPE must be statically-sized type
 */

/* allocate memory for an element of 'DS_ELEM_TYPE',
 *  placed at DS_NAME[unit].
 *if the memory have already been allocated - report ERROR
 *NOTE1: 'DEVICE_ID' is assumed to be set to the current device value
 */

/* free the memory allocated for 'DS_ELEM'*/

/*free the memory allocated for 'unit' entry of DS_NAME data structure*/
/*
 */
int soc_sand_is_long_aligned(
  uint32 word_to_check);

void soc_sand_check_driver_and_device(
  int unit,
  uint32 * error_word);

uint32 soc_sand_get_index_of_max_member_in_array(
  SOC_SAND_IN uint32 array[],
  SOC_SAND_IN uint32 len);
/*
 * }
 */
#endif /* 0 */

shr_error_e utilex_set_field(
  uint32 * reg_val,
  uint32 ms_bit,
  uint32 ls_bit,
  uint32 field_val);

shr_error_e utilex_U8_to_U32(
  uint8 * u8_val,
  uint32 nof_bytes,
  uint32 * u32_val);

shr_error_e utilex_U32_to_U8(
  uint32 * u32_val,
  uint32 nof_bytes,
  uint8 * u8_val);

#if 0
/*
 * {
 */

/*
 * {  SOC_SAND_SYNC_IOS
 * Macro for IO-memory barrier synchronize
 * When accessing to the device "very soon" after writing a value and
 * expecting it to do something. Example indirect trigger.
 */
#if (defined(CPU) && ((CPU)==(PPC860)) && defined(__GNUC__))
#define SOC_SAND_SYNC_IOS   __asm__ __volatile__ ("   eieio")
#elif defined(SOC_PPC_CPU) && defined(__GNUC__)
#define SOC_SAND_SYNC_IOS   __asm__ __volatile__ ("   eieio")
#else
#define SOC_SAND_SYNC_IOS
#endif

#ifndef SOC_SAND_SYNC_IOS
#error "SOC_SAND_SYNC_IOS need to be defined." \
         "  Specifically, in Dune Reference System we use PPC860" \
         "  Consult with the CPU Data Sheet, or your BSP expert, for a similar ASM code."
#endif

void soc_sand_SAND_U32_RANGE_clear(
  SOC_SAND_OUT SOC_SAND_U32_RANGE * info);

void soc_sand_SAND_TABLE_BLOCK_RANGE_clear(
  SOC_SAND_OUT SOC_SAND_TABLE_BLOCK_RANGE * info);

#if SOC_SAND_DEBUG
/* { */
int soc_sand_general_display_err(
  uint32 err_id,
  const char *error_txt);

/*
 * Printing utility.
 * Convert from enumerator to string.
 */
const char *soc_sand_SAND_OP_to_str(
  SOC_SAND_IN SOC_SAND_OP soc_sand_op,
  SOC_SAND_IN uint32 short_format);

/*
 * Print HEX buffer.
 */
void soc_sand_print_hex_buff(
  SOC_SAND_IN char *buff,
  SOC_SAND_IN uint32 buff_byte_size,
  SOC_SAND_IN uint32 nof_bytes_per_line);

/*
 * Prints Band-Width.
 */
void soc_sand_print_bandwidth(
  SOC_SAND_IN uint32 bw_kbps,
  SOC_SAND_IN uint32 short_format);

const char *soc_sand_SAND_SUCCESS_FAILURE_to_string(
  SOC_SAND_IN SOC_SAND_SUCCESS_FAILURE enum_val);

void soc_sand_SAND_U32_RANGE_print(
  SOC_SAND_IN SOC_SAND_U32_RANGE * info);

void soc_sand_SAND_TABLE_BLOCK_RANGE_print(
  SOC_SAND_IN SOC_SAND_TABLE_BLOCK_RANGE * info);

/* } */
#endif

/*
 * } End SOC_SAND_SYNC_IOS
 */

 /*
  * }
  */
#endif /* 0 */

/* } */
#endif /* UTILEX_FRAMEWORK_H_INCLUDED */
