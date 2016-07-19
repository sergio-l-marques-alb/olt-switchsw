/* $Id: petra_api_diagnostics.h,v 1.10 Broadcom SDK $
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
*/

#ifndef __SOC_PETRA_API_DIAGNOSTICS_INCLUDED__
/* { */
#define __SOC_PETRA_API_DIAGNOSTICS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_64cnt.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/TMC/tmc_api_diagnostics.h>
#include <soc/dpp/Petra/petra_api_mgmt.h>
#include <soc/dpp/Petra/petra_api_end2end_scheduler.h>
#include <soc/dpp/Petra/petra_api_ingress_packet_queuing.h>
#include <soc/dpp/Petra/petra_api_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/petra_api_egr_queuing.h>
#include <soc/dpp/Petra/petra_chip_defines.h>
#include <soc/dpp/Petra/petra_framework.h>
#include <soc/dpp/Petra/petra_api_statistics.h>
#include <soc/dpp/Petra/petra_api_ports.h>
#include <soc/dpp/Petra/petra_api_interrupt_service.h>
#include <soc/dpp/Petra/petra_tbl_access.h>

#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#endif /* LINK_PB_LIBRARIES */

/* } */
/*************
 * DEFINES   *
 *************/
/* { */



#define SOC_PETRA_DIAG_IDRAM_WORD_NOF_BITS   (256)
#define SOC_PETRA_DIAG_IDRAM_WORD_NOF_UINT32S (SOC_PETRA_DIAG_IDRAM_WORD_NOF_BITS / SOC_SAND_NOF_BITS_IN_UINT32)

#define SOC_PETRA_DRAM_DLL_RAM_TABLE_SIZE                          (0x80)


/* $Id: petra_api_diagnostics.h,v 1.10 Broadcom SDK $
 * Run QDR BIST till stop/get-result API
 * is called (soc_petra_diag_qdr_BIST_test_result_get)
 */
#define SOC_PETRA_DIAG_QDR_BIST_INFINITE_TEST 0
/*     Number of uint32s in a QDR BIST pattern of 256 bits.      */
#define  SOC_PETRA_DIAG_QDR_BIST_PATTERN_SIZE_IN_UINT32S (8)

/*
 * Run QDR BIST till stop/get-result API
 * is called (soc_petra_diag_dram_BIST_test_result_get)
 */
#define SOC_PETRA_DIAG_DRAM_BIST_INFINITE_TEST 0

#define SOC_PA_DIAG_TBLS_DUMP_ALL_BLOCKS SOC_PETRA_NOF_MODULES
#ifdef LINK_PB_LIBRARIES
#define SOC_PB_DIAG_TBLS_DUMP_ALL_BLOCKS SOC_PB_NOF_MODULES
#else
#define SOC_PB_DIAG_TBLS_DUMP_ALL_BLOCKS SOC_PA_DIAG_TBLS_DUMP_ALL_BLOCKS
#endif
#define SOC_PETRA_DIAG_TBLS_DUMP_ALL_BLOCKS \
  SOC_PETRA_CST_VALUE_DISTINCT(DIAG_TBLS_DUMP_ALL_BLOCKS, uint32)

#define SOC_PETRA_DIAG_TBLS_DUMP_ALL_TBLS SOC_SAND_U32_MAX

/*
 *  DRAM status
 */
#define SOC_PETRA_DIAG_NOF_DDR_TRAIN_SEQS               8
#define SOC_PETRA_DIAG_NOF_DLLS_PER_DRAM                4

/*
 * Maximum size of received/sent packets in bytes
 */
#define SOC_PETRA_DIAG_LBG_PAYLOAD_BYTE_SIZE         (256)

/*     Number of DQSs.                                         */
#define  DIAG_DRAM_NOF_DQSS 4

/*     Select all SMSs.                                        */
#define SOC_PA_DIAG_SMS_ALL               (42)

/*     Number of SMSes in Soc_petra                                */
#define  SOC_PA_DIAG_NOF_SMS              (41)

#ifdef LINK_PB_LIBRARIES
  #define SOC_PB_DIAG_SMS_ALL             (43)
#define   SOC_PB_DIAG_NOF_SMS             (42)
#else
  #define SOC_PB_DIAG_SMS_ALL             SOC_PA_DIAG_SMS_ALL
  #define SOC_PB_DIAG_NOF_SMS             SOC_PA_DIAG_NOF_SMS
#endif

#define SOC_PETRA_DIAG_SMS_ALL \
  SOC_PETRA_CST_VALUE_DISTINCT(DIAG_SMS_ALL, uint32)

#define SOC_PETRA_DIAG_NOF_SMS \
  SOC_PETRA_CST_VALUE_DISTINCT(DIAG_NOF_SMS, uint32)
/* 
 * Maximum between Soc_petra-A, Soc_petra-B, to allow
 * having the same prototype
 */
#define SOC_PETRA_DIAG_SMS_MAX 43


/*     Maximal number of SONEs per SMS in Soc_petra                */
#define  SOC_PETRA_DIAG_NOF_SONE_PER_SMS_MAX (SOC_TMC_DIAG_NOF_SONE_PER_SMS_MAX)

/*     Maximal length (in longs) of the diagnostic chain.      */
#define  SOC_PETRA_DIAG_CHAIN_LENGTH_MAX_IN_UINT32S (SOC_TMC_DIAG_CHAIN_LENGTH_MAX_IN_UINT32S)

/*     Maximal number of SONEs per SMS saved for the result in Soc_petra                */
#define  SOC_PETRA_DIAG_NOF_SONE_SAVED_PER_SMS_MAX (10)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
  SOC_PETRA_DIAG_PKT_HDR_U_TYPE      = 0x8,
  SOC_PETRA_DIAG_PKT_HDR_F_TYPE      = 0x9,
  SOC_PETRA_DIAG_PKT_HDR_M_TYPE      = 0xA,
  SOC_PETRA_DIAG_PKT_HDR_IN_RCY_TYPE = 0xC,

  /*
   * Un-known type.
   * This type do not exist in the device.
   */
  SOC_PETRA_DIAG_PKT_HDR_UNKOWN_TYPE = 0xFF

} SOC_PETRA_DIAG_ING_PKT_HDR_TYPE;

/************************************************************************/
/* BIST info                                                            */
/************************************************************************/

typedef enum
{
  /*
   *  Normal mode. Starting from start address to end address.
   */
  SOC_PETRA_DIAG_QDR_BIST_ADDR_MODE_NORMAL=0,
  /*
   *  Every write command the address is shifted in cyclic
   *  manner one bit left. Address is reset if it exceeds End
   *  Address. Intended for walking one / walking zero tests.
   */
  SOC_PETRA_DIAG_QDR_BIST_ADDR_MODE_ADDRESS_SHIFT=1,
  /*
   *  Writes walking ones and zeros on the address bus and
   *  finds if any address bit is defective. If this mode
   *  chosen for address selection then it also override the
   *  data mode.
   */
  SOC_PETRA_DIAG_QDR_BIST_ADDR_MODE_ADDRESS_TEST=2,

  SOC_PETRA_DIAG_NOF_QDR_BIST_ADDR_MODES=3
}SOC_PETRA_DIAG_QDR_BIST_ADDR_MODE;


typedef enum
{
  /*
   *  Normal mode. Write incrementally each of the pattern
   *  words.
   */
  SOC_PETRA_DIAG_QDR_BIST_DATA_MODE_NORMAL=0,
  /*
   *  Every write command one-bit data from the BistPattern is
   *  duplicated on the entire data bus. For one burst of 2
   *  commands, 2 consecutive bits are used. All 32b of every
   *  BistPattern word are used in this mode.
   */
  SOC_PETRA_DIAG_QDR_BIST_DATA_MODE_PATTERN_BIT=1,
  /*
   *  Every write command is random data, generated by
   *  x^31+x^28+1 polynomial. For one burst of two commands, 2
   *  consecutive 26b data writes are done.
   */
  SOC_PETRA_DIAG_QDR_BIST_DATA_MODE_RANDOM=2,
  /*
   *  Every write command the data is shifted in cyclic manner
   *  one bit left. For one burst of 2 commands, 2 consecutive
   *  shifts are done. Only BistPattern0 word is used in this
   *  mode. Intended for walking one / walking zero tests.
   */
  SOC_PETRA_DIAG_QDR_BIST_DATA_MODE_DATA_SHIFT=3,

  SOC_PETRA_DIAG_NOF_QDR_BIST_DATA_MODES=4
}SOC_PETRA_DIAG_QDR_BIST_DATA_MODE;


typedef enum
{
  /*
   *  Fill the data to write by 1010101... (Bits). The DATA_MODE
   *  may use this data pattern in different ways see
   *  SOC_PETRA_DIAG_QDR_BIST_DATA_MODE. Random mode ignores these
   *  values.
   */
  SOC_PETRA_DIAG_BIST_DATA_PATTERN_DIFF=0,
  /*
   *  Fill the data to write by 11111111... (Bits). The DATA_MODE
   *  may use this data pattern in different ways see
   *  SOC_PETRA_DIAG_QDR_BIST_DATA_MODE. Random mode ignores these
   *  values.
   */
  SOC_PETRA_DIAG_BIST_DATA_PATTERN_ONE=1,
  /*
   *  Fill the data to write by 00000000... (Bits). The DATA_MODE
   *  may use this data pattern in different ways see
   *  SOC_PETRA_DIAG_QDR_BIST_DATA_MODE. Random mode ignores these
   *  values.
   */
  SOC_PETRA_DIAG_BIST_DATA_PATTERN_ZERO=2,
  /*
   *	This pattern is used for QDR initialization
   *  in Parity protection mode.
   */
  SOC_PETRA_DIAG_BIST_DATA_PATTERN_P1=3,
  /*
   *	This pattern is used for QDR initialization
   *  in ECC protection mode.
   */
  SOC_PETRA_DIAG_BIST_DATA_PATTERN_P2 = 4,
  /*
   *  Fill the data to write with a user-defined pattern. The
   *  DATA_MODE may use this data pattern in different ways
   *  see DIAG_QDR_BIST_DATA_MODE. Random mode ignores these
   *  values.
   */
  SOC_PETRA_DIAG_BIST_DATA_PATTERN_USER_DEF = 5,
  /*
   *  Number of types in SOC_PETRA_DIAG_BIST_DATA_PATTERN
   */
  SOC_PETRA_DIAG_NOF_BIST_DATA_PATTERNS = 6
}SOC_PETRA_DIAG_BIST_DATA_PATTERN;

typedef enum
{
  /*
   *  In this mode, the pattern is already configured to get a
   *  bottom threshold on the window size (worst case).
   */
  SOC_PETRA_DIAG_QDR_PATTERN_MODE_RANDOM = 0,
  /*
   *  In this mode, the pattern is alternating zeros and ones.
   */
  SOC_PETRA_DIAG_QDR_PATTERN_MODE_DIFF = 1,
  /*
   *  In this mode, the pattern is configurable in the 'data'
   *  field.
   */
  SOC_PETRA_DIAG_QDR_PATTERN_MODE_USER_DEF = 2,
  /*
   *  Number of types in SOC_PETRA_DIAG_QDR_PATTERN_MODE
   */
  SOC_PETRA_DIAG_NOF_QDR_PATTERN_MODES = 3
}SOC_PETRA_DIAG_QDR_PATTERN_MODE;

typedef enum
{
  /*
   *  Normal mode. Write incrementally each of the pattern
   *  words.
   */
  SOC_PETRA_DIAG_DRAM_BIST_DATA_MODE_NORMAL=0,
  /*
   *  The BIST composes the write/read pattern in the
   *  following manner. For each cycle, a different bit is
   *  selected from the BIST pattern register in an ncremental
   *  manner. This bit is duplicated on all of the DRAM data
   *  bus (all DQs receive the same bit). This allows a
   *  controllable pattern that is 256 write cycles long.
   */
  SOC_PETRA_DIAG_DRAM_BIST_DATA_MODE_PATTERN_BIT=1,
  /*
   *  The PRBS will be used to generate the pattern towards
   *  the DRAM.
   */
  SOC_PETRA_DIAG_DRAM_DATA_MODE_RANDOM=2,
  SOC_PETRA_DIAG_NOF_DRAM_BIST_DATA_MODES =3
}SOC_PETRA_DIAG_DRAM_BIST_DATA_MODE;

typedef enum
{
   /*
    * Dumps all device tables.
    */
    SOC_PETRA_DIAG_TBLS_DUMP_MODE_FULL = 0,
   /*
    * Dumps only part of device tables. These are the tables that are relevant for most debug cases.
    */
    SOC_PETRA_DIAG_TBLS_DUMP_MODE_SELECTIVE = 1,
    
    SOC_PETRA_DIAG_NOF_TBLS_DUMP_MODES = 2
} SOC_PETRA_DIAG_TBLS_DUMP_MODE;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Number of Write commands to execute in one full bist
   *  test. Range: 1 - 0xFFFFFFFF. Use
   *  DIAG_QDR_BIST_INFINITE_TEST(0) to run the test till
   *  stop/get_result API is called.
   */
  uint32 nof_cmnds_write;
  /*
   *  Number of Read commands to execute in one full bist
   *  test. Range: 1 - 0xFFFFFFFF. Note: if nof_cmnds_write is
   *  DIAG_QDR_BIST_INFINITE_TEST(0), the test runs till
   *  stop/get_result API is called, and this field has no
   *  affect.
   */
  uint32 nof_cmnds_read;
  /*
   *  start address of the Bist test commands.
   */
  uint32 start_addr;
  /*
   *  End address of the Bist test commands. If end address is
   *  reached, address is reset to the initial write or read
   *  address.
   *  Note: the last address is QDR-size-bytes - 4.
   */
  uint32 end_addr;
  /*
   *  The read offset, in number-of-commands. If non-zero, the
   *  read is performed starting with this offset of commands
   *  from the write commands start_address. The read is only
   *  performed after the appropriate write command.
   */
  uint32 read_offset;
  /*
   *  The way to build the test data from the data patterns.
   */
  SOC_PETRA_DIAG_QDR_BIST_DATA_MODE data_mode;
  /*
   *  The mode to jump between the addresses
   */
  SOC_PETRA_DIAG_QDR_BIST_ADDR_MODE address_mode;
  /*
   *  How to build the initial data to write to the memory,
   *  the data_mode may use this data_patern differently.
   */
  SOC_PETRA_DIAG_BIST_DATA_PATTERN data_pattern;
  /*
   *  If the data pattern is user defined, this field holds
   *  the user's pattern.
   */
  uint32 user_defined_pattern[SOC_PETRA_DIAG_QDR_BIST_PATTERN_SIZE_IN_UINT32S];

} SOC_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the tests finished. Otherwise - the test is
   *  in-progress, or did not start.
   */
  uint8 is_test_finished;
  /*
   *  Tests the QDR 'ready' bit, indicating the QDR interface
   *  is up.
   */
  uint8 is_qdr_up;
  /*
   *  BIST burst error counter. Counts the number of errors
   *  that appeared on one burst (2 commands) reply.
   */
  uint32 burst_err_counter;
  /*
   *  Number of bit errors that appeared on one reply. This
   *  may increase according to the number of bit errors on
   *  the reply.
   */
  uint32 bit_err_counter;
  /*
   *  Number of errors that appeared on one reply. This may
   *  increase by 1 for every reply, regardless of the number
   *  of different bit errors on the reply.
   */
  uint32 reply_err_counter;
  /*
   *  Indicates for each of the 18 bits if there was error. A
   *  bit is set if an error has occurred on it any time
   *  during the test. Each bit is equivalent to a bit on the
   *  external QDR data bus.
   */
  uint32 bits_error_bitmap;
  /*
   *  The address of the last erroneous burst reply
   */
  uint32 last_addr_err;
  /*
   *  The data of the last erroneous burst reply.
   */
  uint32 last_data_err;
}SOC_PETRA_DIAG_QDR_BIST_TEST_RES_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Mode of the pattern used for the BIST commands:
   *  pre-configured or user-specified.
   */
  SOC_PETRA_DIAG_QDR_PATTERN_MODE mode;
  /*
   *  Data of the pattern. Valid ony if the mode is
   *  user-specified.
   */
  uint32 data[SOC_PETRA_DIAG_QDR_BIST_PATTERN_SIZE_IN_UINT32S];

} SOC_PETRA_DIAG_QDR_PATTERN;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Window start point in the half cycle.
   */
  uint32 start;
  /*
   *  Window end point in the half cycle.
   */
  uint32 end;
  /*
   *  Position of the sampling point in the half cycle. Must
   *  be far from the window boundaries for a valid window.
   */
  uint32 sampling_point;
  /*
   *  Size of the half cycle.
   */
  uint32 total_size;
  /*
   *  Ratio between the window size and the (total) half cycle
   *  size. Represents an indication of the board
   *  quality. Type: percentage. Expected ratio: between 50%
   *  and 70%.
   */
  uint32 ratio_percent;

} SOC_PETRA_DIAG_QDR_WINDOW_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If False, the test runs indefinitely (until the
   *  mechanism BIST is disabled or the get result function is
   *  called). Otherwise, it runs only once. writes_per_cycle
   */
  uint8 is_finite_nof_cmnds;
  /*
   *  Number of Write commands per cycle. Range 0-225. 0 then
   *  no write will be performed.
   */
  uint32 writes_per_cycle;
  /*
   *  Number of Read commands per cycle. Range 0-225. 0 then
   *  no Read will be performed.
   */
  uint32 reads_per_cycle;
  /*
   *  Start address (offset) of the Bist test commands.
   *  The address must be a multiple of 32 Bytes.
   *  Units: Bytes. Range: 0 - DRAM size-32.
   */
  uint32 start_addr;
  /*
   *  End address (offset) of the Bist test commands. If end address is
   *  reached, address is reset to the initial write or read
   *  address. The format is the same as the start address.
   *  For example, for 1 Gb interface, set to 0x7FFFFE0.
   *  For 2 Gb interface, set to 0xFFFFFE0.
   */
  uint32 end_addr;
  /*
   *  The way to build the test data from the data patterns.
   */
  SOC_PETRA_DIAG_DRAM_BIST_DATA_MODE data_mode;
  /*
   *  How to build the initial data to write to the memory,
   *  the data_mode may use this data_pattern differently.
   */
  SOC_PETRA_DIAG_BIST_DATA_PATTERN data_pattern;
}SOC_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   * If TRUE, the tests finished.
   * Otherwise - the test is in-progress, or did not start.
   */
   uint8 is_test_finished;

  /*
   *  Tests the DRAM 'ready' bit, indicating
   *  the DRAM interface is up
   */
   uint8 is_dram_up;

  /*
   *  Number of errors that appeared on one reply. This may
   *  increase by 1 for every reply, regardless of the number
   *  of different bit errors on the reply.
   */
  uint32 reply_err_counter;
}SOC_PETRA_DIAG_DRAM_BIST_TEST_RES_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Address to written in. Units: bytes. Alignment: 32
   *  bytes. The value must be inferior to the DRAM size - 32
   *  bytes.
   */
  uint32 address;
  /*
   *  If true, the buffer is 256 bits long. Otherwise, it is
   *  32 bits long and is duplicated 8 times to form the
   *  pattern.
   */
  uint8 is_data_size_bits_256_not_32;
  /*
   *  If true, the actions (writing / reading) are performed
   *  until stopped by the user by disabling the BIST.
   */
  uint8 is_infinite_nof_actions;
}SOC_PETRA_DIAG_DRAM_ACCESS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If true, all the reading actions have happened and for
   *  each address, the expected pattern has been found.
   */
  uint8 success;
  /*
   *  This field has 32 bits. If bit N equals '1', it
   *  indicates that at least in one address with a failed
   *  comparison, a difference has been found in the bit N, or
   *  N + 32,..., or N + 224 during the comparison with the
   *  expected pattern.
   */
  uint32 error_bits_global;
  /*
   *  Indicates the number of addresses whose pattern was
   *  found different during the comparison with the expected
   *  pattern.
   */
  uint32 nof_addr_with_errors;
}SOC_PETRA_DIAG_DRAM_READ_COMPARE_STATUS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *
   */
  uint8  valid;
  /*
   *
   */
  uint32 queue_qrtt;
  /*
   *
   */
  uint32 queue_id;
  /*
   *
   */
  uint32 flow_qrtt;
  /*
   *
   */
  uint32 flow_id;
  /*
   *
   */
  uint8 is_composite;
  /*
   *
   */
  uint32 sys_phy_port;
  /*
   *
   */
  uint32 dst_fap_id;
  /*
   *
   */
  uint32 dst_prt_id;
}SOC_PETRA_DIAG_PACKET_WALKTROUGH;

typedef union
{
  /*
   *
   */
  struct
  {
    SOC_SAND_MAGIC_NUM_VAR
    /*
     *
     */
    uint32  destination_id;
    /*
     *
     */
    uint32 drp;
    /*
     *
     */
    uint32 snoop_cmd;
    /*
     *
     */
    uint32 class_val;
  } unicast;

  /*
   *
   */
  struct
  {
    SOC_SAND_MAGIC_NUM_VAR
    /*
     *
     */
    uint32 flow_id;
    /*
     *
     */
    uint32 drp;
    /*
     *
     */
    uint32 snoop_cmd;
  } flow;

  /*
   *
   */
  struct
  {
    SOC_SAND_MAGIC_NUM_VAR
    /*
     *
     */
    uint32 source_port_addr;
    /*
     *
     */
    uint32 multicast_id;
    /*
     *
     */
    uint32 drp;
    /*
     *
     */
    uint32 snoop_cmd;
    /*
     *
     */
    uint32 exclude_src;
    /*
     *
     */
    uint32 class_val;
  } multicast;

} SOC_PETRA_DIAG_ING_PKT_HDR_DATA;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Up to 256 byte Data to carry on packet
   */
  uint8 data[SOC_PETRA_DIAG_LBG_PAYLOAD_BYTE_SIZE];
  /*
   *  Number of bytes in packet
   */
  uint32 data_byte_size;

}SOC_PETRA_DIAG_LBG_PACKET_PATTERN;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
 /*
  * Packet data and size
  * If packet size is less then pattern size, only a part of pattern is taken.
  * If packet size is more then pattern size, the pattern is duplicated up
  * to the packet size.
  */
  SOC_PETRA_DIAG_LBG_PACKET_PATTERN pattern;
 /*
  * Packet size in bytes
  */
  uint32 packet_size;
 /*
  * number of packets to send from the CPU.
  * These packets will be sent in a burst.
  * and will be forwarded in the defined loop
  */
  uint32 nof_packets;
}SOC_PETRA_DIAG_LBG_TRAFFIC_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
 /*
  * Soc_petra counters
  */
  SOC_PETRA_STAT_ALL_STATISTIC_COUNTERS counters;
 /*
  * traffic rate in the ingress (Kbps)
  */
  uint32 ingress_rate;
 /*
  * traffic rate in the Egress (Kbps)
  */
  uint32 egress_rate;
 /*
  * credits in (Kbps)
  */
  uint32 credits;
}SOC_PETRA_DIAG_LBG_RESULT_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
 /*
  * ports that are part of the LBG test.
  */
  uint32 ports[SOC_PETRA_NOF_FAP_PORTS];
 /*
  *  number of ports.
  *  Soc_petra-A: Range: 0 - 79.
  *  Soc_petra-B: Range: 0 - 63.
  */
  uint32 nof_ports;
}SOC_PETRA_DIAG_LBG_PATH_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
   /*
    * the path packet will go through
    */
    SOC_PETRA_DIAG_LBG_PATH_INFO     path;
}SOC_PETRA_DIAG_LBG_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *
   */
  uint32 ddl_control_0;
  /*
   *
   */
  uint32 ddl_control_1;
  /*
   *
   */
  uint32 ddl_control_2;
  /*
   *
   */
  uint8 rnd_trp;
  /*
   *
   */
  uint8 rnd_trp_diff;
  /*
   *
   */
  uint8 dll_init_done;
  /*
   *
   */
  uint8 dll_ph_dn;
  /*
   *
   */
  uint8 dll_ph_up;
  /*
   *
   */
  uint8 main_ph_sel;
  /*
   *
   */
  uint8 ph2sel;
  /*
   *
   */
  uint8 hc_sel_vec;
  /*
   *
   */
  uint8 qc_sel_vec;
  /*
   *
   */
  uint8 sel_vec;
  /*
   *
   */
  uint8 sel_hg;
  /*
   *
   */
  uint8 ph_sel_hc_up;
  /*
   *
   */
  uint8 ins_dly_min_vec;
  /*
   *
   */
  uint8 ddl_init_main_ph_sel_ofst;
  /*
   *
   */
  uint8 ddl_ph_sel_hc_up;
  /*
   *
   */
  uint8 ddl_train_trig_up_limit;
  /*
   *
   */
  uint8 ddl_train_trig_dn_limit;
  /*
   *
   */
  uint8 ph_sel_err;
  /*
   *
   */
  uint8 dly_max_min_mode;
  /*
   *
   */
  uint8 ph_sel;
  /*
   *
   */
  uint8 max_sel_vec;
  /*
   *
   */
  uint8 min_sel_vec;
  
}SOC_PETRA_DIAG_DLL_STATUS_INFO;

typedef struct
{
  uint32 dll_ram[2];
} SOC_PETRA_DIAG_DLL_RAM_TBL_DATA;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *
   */
  uint32 training_seq;
  /*
   *
   */
  uint32 calibration_st;
  /*
   *
   */
  uint32 ddl_periodic_training;
  /*
   *
   */
  uint8 dll_mstr_s;
  /*
   *
   */
  uint32 ddr_training_sequence[SOC_PETRA_DIAG_NOF_DDR_TRAIN_SEQS];
  /*
   *  Status of the 4 DLLs.
   */
  SOC_PETRA_DIAG_DLL_STATUS_INFO dll_status[SOC_PETRA_DIAG_NOF_DLLS_PER_DRAM];
  /*
   *
   */
  SOC_PETRA_DIAG_DLL_RAM_TBL_DATA dll_ram_tbl_data[SOC_PETRA_DRAM_DLL_RAM_TABLE_SIZE];
}SOC_PETRA_DIAG_DRAM_STATUS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Bitmap of the erroneous bits (i.e., with a problematic
   *  connectivity) in the training sequence operation.
   */
  uint32 bit_err_bitmap;
  /*
   *  If True, then the DRAM interface has a clocking problem
   *  due to the PLL configuration.
   */
  uint8 is_clocking_err;
  /*
   *  If True, then the respective DQS has a connectivity
   *  error.
   */
  uint8 is_dqs_con_err[DIAG_DRAM_NOF_DQSS];
  /*
   *  If True, then the PHY and DPI initialization is not
   *  done.
   */
  uint8 is_phy_ready_err;
  /*
   *  If True, then the average round trip time is too low.
   */
  uint8 is_rtt_avg_min_err;
  /*
   *  If True, then the average round trip time is too high.
   */
  uint8 is_rtt_avg_max_err;

}  SOC_PETRA_DIAG_DRAM_ERR_INFO;

#define SOC_PETRA_DIAG_SOFT_ERROR_PATTERN_ONE                  SOC_TMC_DIAG_SOFT_ERROR_PATTERN_ONE
#define SOC_PETRA_DIAG_SOFT_ERROR_PATTERN_ZERO                 SOC_TMC_DIAG_SOFT_ERROR_PATTERN_ZERO
#define SOC_PETRA_DIAG_SOFT_ERROR_PATTERN_DIFF1                SOC_TMC_DIAG_SOFT_ERROR_PATTERN_DIFF1
#define SOC_PETRA_DIAG_SOFT_ERROR_PATTERN_DIFF2                SOC_TMC_DIAG_SOFT_ERROR_PATTERN_DIFF2
#define SOC_PETRA_DIAG_NOF_SOFT_ERROR_PATTERNS                 SOC_TMC_NOF_DIAG_SOFT_ERROR_PATTERNS
typedef SOC_TMC_DIAG_SOFT_ERROR_PATTERN                        SOC_PETRA_DIAG_SOFT_ERROR_PATTERN;

#define SOC_PETRA_DIAG_SOFT_COUNT_TYPE_FAST                    SOC_TMC_DIAG_SOFT_COUNT_TYPE_FAST
#define SOC_PETRA_DIAG_SOFT_COUNT_TYPE_COMPLETE                SOC_TMC_DIAG_SOFT_COUNT_TYPE_COMPLETE
#define SOC_PETRA_DIAG_NOF_SOFT_COUNT_TYPES                    SOC_TMC_NOF_DIAG_SOFT_COUNT_TYPES
typedef SOC_TMC_DIAG_SOFT_COUNT_TYPE                           SOC_PETRA_DIAG_SOFT_COUNT_TYPE;

typedef SOC_TMC_DIAG_SOFT_ERR_INFO                             SOC_PETRA_DIAG_SOFT_ERR_INFO;
typedef SOC_TMC_DIAG_SOFT_ERR_RESULT                           SOC_PETRA_DIAG_SOFT_ERR_RESULT;
typedef SOC_TMC_DIAG_SOFT_SMS_RESULT                           SOC_PETRA_DIAG_SOFT_SMS_RESULT;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_petra_diag_ipt_rate_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT SOC_SAND_64CNT               *rate
  );

uint32
  soc_petra_diag_iddr_set(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    dram_ndx,
    SOC_SAND_IN  uint32    bank_ndx,
    SOC_SAND_IN  uint32     dram_offset,
    SOC_SAND_IN  uint32     *data,
    SOC_SAND_IN  uint32     size
  );

uint32
  soc_petra_diag_iddr_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    dram_ndx,
    SOC_SAND_IN  uint32    bank_ndx,
    SOC_SAND_IN  uint32     dram_offset,
    SOC_SAND_OUT uint32     *data,
    SOC_SAND_IN  uint32     size
  );

uint32
  soc_petra_diag_regs_dump(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  start,
    SOC_SAND_IN uint32  end
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_tbls_dump
* TYPE:
*   PROC
* FUNCTION:
*     dump tables; one table; tables of one block, all tables
* INPUT:
*  SOC_SAND_IN  uint8    block_id -
*    the block ID that include the table/tables to print
*    Set SOC_PETRA_DIAG_TBLS_DUMP_ALL_BLOCKS to dump all blocks.
*  SOC_SAND_IN  uint32   tbl_base_addr -
*    the base address of the table to dump.
*    Set to SOC_PETRA_DIAG_TBLS_DUMP_ALL_TBLS to dump all tables in a
*    block blocks
*  SOC_SAND_IN  uint8  print_zero -
*    whether to print zero entries.
*    If TRUE, zero entries are printed. otherwise zero entries are ignored.
* REMARKS:
*   1. block_id = SOC_PETRA_DIAG_TBLS_DUMP_ALL_BLOCKS AND
*      tbl_base_addr = SOC_PETRA_DIAG_TBLS_DUMP_ALL_TBLS
*      Will dump all tables in all blocks.
*   2. block_id = "block_i" AND
*      tbl_base_addr = SOC_PETRA_DIAG_TBLS_DUMP_ALL_TBLS
*      Will dump all tables in the block "block_i".
*   3. block_id = "block_i" OR block_id = SOC_PETRA_DIAG_TBLS_DUMP_ALL_TBLS AND
*      tbl_base_addr = "table_base_address_val"
*      Will dump the table with the given base address.
*      i.e. to print one table: set tbl_base_addr to be the base address
*      of the table. and block_id to be the correct block id or SOC_PETRA_DIAG_TBLS_DUMP_ALL_BLOCKS
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_tbls_dump(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8    block_id,
    SOC_SAND_IN  uint32   tbl_base_addr,
    SOC_SAND_IN  uint8  print_zero
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_tbls_dump_all
* TYPE:
*   PROC
* FUNCTION:
*     dump all tables
* INPUT:
*  SOC_SAND_IN  uint8  print_zero -
*    whether to print zero entries.
*    If TRUE, zero entries are printed. otherwise zero entries are ignored.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_tbls_dump_all(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8  print_zero
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_dev_tables_dump
* TYPE:
*   PROC
* FUNCTION:
*     dump all tables
* INPUT:
*  SOC_SAND_IN  uint8  print_zero -
*    whether to print zero entries.
*    If TRUE, zero entries are printed. otherwise zero entries are ignored.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_dev_tables_dump(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_TBLS_DUMP_MODE dump_mode
  );


/*********************************************************************
* NAME:
*     soc_petra_nif_diag_last_packet_get
* TYPE:
*   PROC
* FUNCTION:
*     Read the last packet header entered the NIF and return
*     the values of the ingress ITMH header fields.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_PORTS_ITMH          *last_incoming_header -
*     Fields of the last ITMH header
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
 *   1. If the packet is processed with ingress shaping, then
 *   the returned ITMH corresponds to the one of the ingress
 *   shaping. 2. For Soc_petra-A, not valid for Raw and Ethernet
 *   ports.
*********************************************************************/

uint32
  soc_petra_nif_diag_last_packet_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_PORTS_ITMH          *last_incoming_header
  );

/************************************************************************/
/*  BIST API                                                            */
/************************************************************************/
/*********************************************************************
* NAME:
*     soc_petra_diag_dram_write
* TYPE:
*   PROC
* FUNCTION:
*     Write a pattern of 256 bits long into the DRAM via the
*     BIST mechanism.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  dram_ndx -
*     DRAM index.
*  SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_ACCESS_INFO *info -
*     Information for writing in the DRAM.
*  SOC_SAND_IN  uint32                  *buffer -
*     The pattern to write.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_diag_dram_write(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_ACCESS_INFO *info,
    SOC_SAND_IN  uint32                  *buffer
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_dram_read_and_compare
* TYPE:
*   PROC
* FUNCTION:
*     Indicates if a pattern is written in the specified
*     addresses by reading and comparing the written pattern
*     with the expected data via the BIST mechanism.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  dram_ndx -
*     DRAM index.
*  SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_ACCESS_INFO *info -
*     Information for reading with a comparison in the DRAM.
*  SOC_SAND_IN  uint32                  *buff_expected -
*     Expected pattern. The actual value is compared to this
*     pattern.
*  SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_READ_COMPARE_STATUS *status -
*     Indicates if errors have been found and their
*     characteristics.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_diag_dram_read_and_compare(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_ACCESS_INFO *info,
    SOC_SAND_IN  uint32                  *buff_expected,
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_READ_COMPARE_STATUS *status
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_qdr_BIST_test_start
* TYPE:
*   PROC
* FUNCTION:
*     Run BIST test with the given parameters.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO *test_info -
*     Information for running the BIST test.
* REMARKS:
*     - All traffic and indirect access must stop before run
*     BIST test.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_qdr_BIST_test_start(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO *test_info
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_qdr_BIST_test_result_get
* TYPE:
*   PROC
* FUNCTION:
*     Stop BIST test and return the result.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_DIAG_QDR_BIST_TEST_RES_INFO *res_info -
*     Result of the test.
* REMARKS:
*     - All traffic and indirect access must stop before run
*     BIST test.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_qdr_BIST_test_result_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_OUT SOC_PETRA_DIAG_QDR_BIST_TEST_RES_INFO  *res_info
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_dram_BIST_test_start
* TYPE:
*   PROC
* FUNCTION:
*     Run DRAM BIST test with the given parameters.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  dram_ndx -
*     Dram index.
*  SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO *test_info -
*     Information for running the BIST test.
* REMARKS:
*     - All traffic and indirect access must stop before run
*     BIST test. - The accessed addresses are the ones with a
*     value between test_info.start_addr and
*     test_info.end_addr.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_dram_BIST_test_start(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO *test_info
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_dram_BIST_test_result_get
* TYPE:
*   PROC
* FUNCTION:
*     Stop DRAM BIST test and return the result.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 dram_ndx -
*     Memory index.
*  SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_BIST_TEST_RES_INFO *res_info -
*     Result of the test.
* REMARKS:
*     - All traffic and indirect access must stop before run
*     BIST test. - if the test start when infinite mode then
*     this operation will stop the BIST and return the result.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_dram_BIST_test_result_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_BIST_TEST_RES_INFO *res_info
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_dll_status_get
* TYPE:
*   PROC
* FUNCTION:
*     This function is used as a diagnostics tool that gives
*     indications about the dll status.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 dram_ndx -
*     DRAM index. Range: 0 - 5.
*  SOC_SAND_IN  uint32                 dll_ndx -
*     DLL index. Range: 0 - 3.
*  SOC_SAND_OUT SOC_PETRA_DIAG_DLL_STATUS_INFO *dll_stt_info -
*     Different indications about the dll.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_diag_dll_status_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dll_ndx,
    SOC_SAND_OUT SOC_PETRA_DIAG_DLL_STATUS_INFO *dll_stt_info
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_dram_status_get
* TYPE:
*   PROC
* FUNCTION:
*     Get DRAM diagnostic.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 dram_ndx -
*     DRAM index. Range: 0 - 5.
*  SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_STATUS_INFO *dram_status -
*     Different indications about the DRAM.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_diag_dram_status_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_STATUS_INFO *dram_status
  );

/*********************************************************************
* NAME:
*   soc_petra_diag_dram_diagnostic_get
* TYPE:
*   PROC
* FUNCTION:
*   Get diagnostic for the DRAM interface training sequence.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  uint32                                 dram_ndx -
*     DRAM index. Range: 0 - 5.
*   SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_ERR_INFO                  *error_info -
*     Different possible errors in the DRAM interface.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_dram_diagnostic_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 dram_ndx,
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_ERR_INFO                  *error_info
  );

/************************************************************************/
/* LBG APIs                                                             */
/************************************************************************/

/*********************************************************************
* NAME:
*     soc_petra_diag_lbg_conf_set
* FUNCTION:
*     - save previous relevant configuration of Soc_petra.
*     - configure the Soc_petra to perform LBG test
*       -Raw port snake configuration.
*
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* REMARKS:
*     - All traffic must stop before run LBG test
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_lbg_conf_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_LBG_INFO                *lbg_info
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_lbg_traffic_send
* FUNCTION:
*  send traffic using CPU.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* REMARKS:
*     - All traffic must stop before run LBG test
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_lbg_traffic_send(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_LBG_TRAFFIC_INFO        *traffic_info
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_lbg_result_get
* FUNCTION:
*  Get the counters and traffic rate. resulted from the last call
*  of soc_petra_diag_lbg_traffic_send.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_lbg_result_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_OUT  SOC_PETRA_DIAG_LBG_RESULT_INFO        *res_info
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_lbg_close
* FUNCTION:
*  Close the LBG test and configure the configuration
*  as were before the call to soc_petra_diag_lbg_conf_set.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_lbg_close(
    SOC_SAND_IN  int                          unit
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_interrupt_to_recovery_action
* FUNCTION:
*  This function is used to describe what is the recommended recovery action
*  that should be taken, when a given interrupt was indicated.
*  Note: this function only guidelines the user for the recommended action
*  without running any specific action.
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE      cause -
*     Identifier of the interrupt cause.
*  SOC_SAND_OUT SOC_PETRA_INT_RECOVERY_ACTION *action -
*     Recommended recovery action.
* RETURNS
*  None.
*********************************************************************/
void
  soc_petra_diag_interrupt_to_recovery_action(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PETRA_INTERRUPT_CAUSE              cause,
    SOC_SAND_OUT SOC_PETRA_INT_RECOVERY_ACTION          *action
  );

/*********************************************************************
* NAME:
 *   soc_petra_diag_qdr_window_validity_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   A diagnostic tool that determines the QDR window
 *   validity by using a series of BIST commands.
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_DIAG_QDR_PATTERN              *pattern -
 *     Selects the pattern to use in the BIST. Both a
 *     user-defined and an automatic random pattern are
 *     supported.
 *   SOC_SAND_OUT SOC_PETRA_DIAG_QDR_WINDOW_INFO          *window_info -
 *     Parameters characterizing the QDR window validity.
 * REMARKS:
 *   1. The expected ratio is between 50% and 70%.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_qdr_window_validity_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_PATTERN              *pattern,
    SOC_SAND_OUT SOC_PETRA_DIAG_QDR_WINDOW_INFO          *window_info
  );
/*********************************************************************
* NAME:
 *   soc_petra_diag_soft_error_test_start
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Start the soft-error test analyzing the memory validity.
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO            *info -
 *     Parameters of the test: pattern format and SMS to be
 *     used for the tests.
 * REMARKS:
 *   The test overrides all the memory values. The device
 *   must be reset after performing this test.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_soft_error_test_start(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO            *info
  );

/*
 *  Run MBIST on a specified SMS.
 *  To run on all memories, use SOC_PETRA_DIAG_SMS_ALL as the sms value.
 *  SMS index memory to write the pattern in. 
 *  sms is in range: 1 -  29 (SOC_PETRA_DIAG_SMS_ALL).
 */
uint32
  soc_petra_diag_mbist_run(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32     sms,
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_SMS_RESULT   *sms_result[SOC_PETRA_DIAG_SMS_MAX]
  );

/*********************************************************************
* NAME:
 *   soc_petra_diag_soft_error_test_result_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Start the soft-error test analyzing the memory validity.
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO            *info -
 *     Parameters of the test: pattern format and SMS to be
 *     used for the tests.
 *   SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_COUNT_TYPE          count_type -
 *     Count type of the errors.
 *   SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_SMS_RESULT          **sms_result -
 *     Pointers of SMS results. The user must allocate the
 *     pointers before calling the API.
 *   SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_ERR_RESULT          *glbl_result -
 *     Global error result when added on all the analyzed
 *     SMSes.
 * REMARKS:
 *   1. The test must be previously started via the
 *   soc_petra_diag_soft_error_test_start API.2. The number of
 *   SMS results pointers to be allocated is equal to the
 *   number of SMS to analyze. The SMS result is (maximally)
 *   an array of
 *   [SOC_PETRA_DIAG_NOF_SMS][SOC_PETRA_DIAG_NOF_SONE_PER_SMS_MAX]3.
 *   The first SONE result per SMS result is the global SMS
 *   result (the only relevant in case of 'FAST' count type).
 *   The other SMS results are the per SONE results (1 - up
 *   to 8K)
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_soft_error_test_result_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO            *info,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_COUNT_TYPE          count_type,
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_SMS_RESULT          *sms_result[SOC_PETRA_DIAG_SMS_MAX][SOC_PETRA_DIAG_NOF_SONE_SAVED_PER_SMS_MAX],
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_ERR_RESULT          *glbl_result
  );

void
  soc_petra_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO *info
  );

void
  soc_petra_PETRA_DIAG_QDR_BIST_TEST_RES_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_QDR_BIST_TEST_RES_INFO *info
  );

void
  soc_petra_PETRA_DIAG_QDR_PATTERN_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_QDR_PATTERN *info
  );

void
  soc_petra_PETRA_DIAG_QDR_WINDOW_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_QDR_WINDOW_INFO *info
  );

void
  soc_petra_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO *info
  );

void
  soc_petra_PETRA_DIAG_DRAM_BIST_TEST_RES_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_BIST_TEST_RES_INFO *info
  );

void
  soc_petra_PETRA_DIAG_DRAM_ACCESS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_ACCESS_INFO *info
  );

void
  soc_petra_PETRA_DIAG_DRAM_READ_COMPARE_STATUS_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_READ_COMPARE_STATUS *info
  );

void
  soc_petra_PETRA_DIAG_LBG_TRAFFIC_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_LBG_TRAFFIC_INFO *info
  );

void
  soc_petra_PETRA_DIAG_LBG_RESULT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_LBG_RESULT_INFO *info
  );

void
  soc_petra_PETRA_DIAG_LBG_PATH_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_LBG_PATH_INFO *info
  );

void
  soc_petra_PETRA_DIAG_LBG_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_LBG_INFO *info
  );

void
  soc_petra_PETRA_DIAG_DLL_STATUS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_DLL_STATUS_INFO *info
  );

void
  soc_petra_PETRA_DIAG_DRAM_STATUS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_STATUS_INFO *info
  );

void
  soc_petra_PETRA_DIAG_DRAM_ERR_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_ERR_INFO *info
  );

void
  soc_petra_PETRA_DIAG_LBG_PACKET_PATTERN_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_LBG_PACKET_PATTERN *info
  );

void
  soc_petra_PETRA_DIAG_SOFT_ERR_INFO_clear(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_ERR_INFO *info
  );

void
  soc_petra_PETRA_DIAG_SOFT_ERR_RESULT_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_ERR_RESULT *info
  );

void
  soc_petra_PETRA_DIAG_SOFT_SMS_RESULT_clear(
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_SMS_RESULT *info
  );


#if SOC_PETRA_DEBUG_IS_LVL1



void
  soc_petra_PETRA_DIAG_LBG_TRAFFIC_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_LBG_TRAFFIC_INFO *info
  );


void
  soc_petra_PETRA_DIAG_LBG_RESULT_INFO_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN SOC_PETRA_DIAG_LBG_RESULT_INFO *info
  );

void
  soc_petra_PETRA_DIAG_LBG_PATH_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_LBG_PATH_INFO *info
  );

void
  soc_petra_PETRA_DIAG_LBG_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_LBG_INFO *info
  );


uint32
  soc_petra_egq_resources_print(
    int unit
  );


void
  soc_petra_diag_packet_walktrough_print(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   dest_id,
    SOC_SAND_IN  uint32  class_id
  );

void
  soc_petra_diag_all2all_packet_walktrough_print(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   min_dest_id,
    SOC_SAND_IN  uint32   max_dest_id
  );

void
  soc_petra_diag_header_print(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint32          data,
    SOC_SAND_IN  uint8         short_format,
    SOC_SAND_IN  uint8         print_new_line
  );

void
  soc_petra_diag_version_print(
    SOC_SAND_IN  int unit
  );


const char*
  soc_petra_PETRA_DIAG_QDR_BIST_ADDR_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_DIAG_QDR_BIST_ADDR_MODE enum_val
  );



const char*
  soc_petra_PETRA_DIAG_QDR_BIST_DATA_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_DIAG_QDR_BIST_DATA_MODE enum_val
  );



const char*
  soc_petra_PETRA_DIAG_BIST_DATA_PATTERN_to_string(
    SOC_SAND_IN SOC_PETRA_DIAG_BIST_DATA_PATTERN enum_val
  );

const char*
  soc_petra_PETRA_DIAG_QDR_PATTERN_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_PATTERN_MODE enum_val
  );

const char*
  soc_petra_PETRA_DIAG_DRAM_BIST_DATA_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_DIAG_DRAM_BIST_DATA_MODE enum_val
  );

const char*
  soc_petra_PETRA_DIAG_SOFT_ERROR_PATTERN_to_string(
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERROR_PATTERN enum_val
  );

const char*
  soc_petra_PETRA_DIAG_SOFT_COUNT_TYPE_to_string(
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_COUNT_TYPE enum_val
  );

void
  soc_petra_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO *info
  );



void
  soc_petra_PETRA_DIAG_QDR_BIST_TEST_RES_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_QDR_BIST_TEST_RES_INFO *info
  );

void
  soc_petra_PETRA_DIAG_QDR_PATTERN_print(
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_PATTERN *info
  );

void
  soc_petra_PETRA_DIAG_QDR_WINDOW_INFO_print(
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_WINDOW_INFO *info
  );

void
  soc_petra_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO *info
  );



void
  soc_petra_PETRA_DIAG_DRAM_BIST_TEST_RES_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_DRAM_BIST_TEST_RES_INFO *info
  );


void
  soc_petra_PETRA_DIAG_DRAM_ACCESS_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_DRAM_ACCESS_INFO *info
  );



void
  soc_petra_PETRA_DIAG_DRAM_READ_COMPARE_STATUS_print(
    SOC_SAND_IN SOC_PETRA_DIAG_DRAM_READ_COMPARE_STATUS *info
  );



void
  soc_petra_PETRA_DIAG_DLL_STATUS_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_DLL_STATUS_INFO *info
  );



void
  soc_petra_PETRA_DIAG_DRAM_STATUS_INFO_print(
    SOC_SAND_IN SOC_PETRA_DIAG_DRAM_STATUS_INFO *info
  );

void
  soc_petra_PETRA_DIAG_DRAM_ERR_INFO_print(
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_ERR_INFO *info
  );

void
  soc_petra_PETRA_DIAG_LBG_PACKET_PATTERN_print(
    SOC_SAND_IN SOC_PETRA_DIAG_LBG_PACKET_PATTERN *info
  );

void
  soc_petra_PETRA_DIAG_SOFT_ERR_INFO_print(
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO *info
  );

void
  soc_petra_PETRA_DIAG_SOFT_ERR_RESULT_print(
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_RESULT *info
  );

void
  soc_petra_PETRA_DIAG_SOFT_SMS_RESULT_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  sms_ndx,
    SOC_SAND_IN  uint32                  sone_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_SMS_RESULT *info
  );

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_API_DIAGNOSTICS_INCLUDED__*/
#endif
