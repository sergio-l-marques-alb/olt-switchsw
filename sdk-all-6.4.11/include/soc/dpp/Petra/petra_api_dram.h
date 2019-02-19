/* $Id: petra_api_dram.h,v 1.8 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_api_dram.h
*
* MODULE PREFIX:  soc_petra_dram
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement
******************************************************************/

#ifndef __SOC_PETRA_API_DRAM_INCLUDED__
/* { */
#define __SOC_PETRA_API_DRAM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/TMC/tmc_api_dram.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_DRAM_VAL_LSB                                ( 0)
#define SOC_PETRA_DRAM_VAL_MSB                                (30)
#define SOC_PETRA_DRAM_VAL_SHIFT                              (SOC_PETRA_DRAM_VAL_LSB)
#define SOC_PETRA_DRAM_VAL_MASK                               (SOC_SAND_BITS_MASK(SOC_PETRA_DRAM_VAL_MSB, SOC_PETRA_DRAM_VAL_LSB))

#define SOC_PETRA_DRAM_VAL_IS_IN_CLOCKS_LSB                   (31)
#define SOC_PETRA_DRAM_VAL_IS_IN_CLOCKS_MSB                   (31)
#define SOC_PETRA_DRAM_VAL_IS_IN_CLOCKS_SHIFT                 (SOC_PETRA_DRAM_VAL_IS_IN_CLOCKS_LSB)
#define SOC_PETRA_DRAM_VAL_IS_IN_CLOCKS_MASK                  (SOC_SAND_BITS_MASK(SOC_PETRA_DRAM_VAL_IS_IN_CLOCKS_MSB, SOC_PETRA_DRAM_VAL_IS_IN_CLOCKS_LSB))

/*     Number of longs in a DRAM pattern of 256 bits.          */
#define  SOC_PETRA_DRAM_PATTERN_SIZE_IN_UINT32S (8)

#define SOC_PETRA_DRAM_TYPE_GDDR3                               SOC_TMC_DRAM_TYPE_GDDR3
#define SOC_PETRA_DRAM_TYPE_DDR2                                SOC_TMC_DRAM_TYPE_DDR2
#define SOC_PETRA_DRAM_TYPE_DDR3                                SOC_TMC_DRAM_TYPE_DDR3
#define SOC_PETRA_DRAM_NOF_TYPES                                SOC_TMC_DRAM_NOF_TYPES
typedef SOC_TMC_DRAM_TYPE                                       SOC_PETRA_DRAM_TYPE;

#define SOC_PETRA_DRAM_NUM_COLUMNS_256                          SOC_TMC_DRAM_NUM_COLUMNS_256
#define SOC_PETRA_DRAM_NUM_COLUMNS_512                          SOC_TMC_DRAM_NUM_COLUMNS_512
#define SOC_PETRA_DRAM_NUM_COLUMNS_1024                         SOC_TMC_DRAM_NUM_COLUMNS_1024
#define SOC_PETRA_DRAM_NUM_COLUMNS_2048                         SOC_TMC_DRAM_NUM_COLUMNS_2048
#define SOC_PETRA_DRAM_NUM_COLUMNS_4096                         SOC_TMC_DRAM_NUM_COLUMNS_4096
#define SOC_PETRA_DRAM_NUM_COLUMNS_8192                         SOC_TMC_DRAM_NUM_COLUMNS_8192
#define SOC_PETRA_NOF_DRAM_NUMS_COLUMNS                         SOC_TMC_NOF_DRAM_NUMS_COLUMNS
typedef SOC_TMC_DRAM_NUM_COLUMNS                                SOC_PETRA_DRAM_NUM_COLUMNS;                              

#define SOC_PETRA_DRAM_NUM_BANKS_4                              SOC_TMC_DRAM_NUM_BANKS_4
#define SOC_PETRA_DRAM_NUM_BANKS_8                              SOC_TMC_DRAM_NUM_BANKS_8
#define SOC_PETRA_NOF_DRAM_NUM_BANKS                            SOC_TMC_NOF_DRAM_NUM_BANKS
typedef SOC_TMC_DRAM_NUM_BANKS                                  SOC_PETRA_DRAM_NUM_BANKS;

#define SOC_PETRA_DRAM_AP_POSITION_08                           SOC_TMC_DRAM_AP_POSITION_08
#define SOC_PETRA_DRAM_AP_POSITION_09                           SOC_TMC_DRAM_AP_POSITION_09
#define SOC_PETRA_DRAM_AP_POSITION_10                           SOC_TMC_DRAM_AP_POSITION_10
#define SOC_PETRA_DRAM_AP_POSITION_11                           SOC_TMC_DRAM_AP_POSITION_11
#define SOC_PETRA_DRAM_AP_POSITION_12                           SOC_TMC_DRAM_AP_POSITION_12
#define SOC_PETRA_NOF_DRAM_AP_POSITIONS                         SOC_TMC_NOF_DRAM_AP_POSITIONS
typedef SOC_TMC_DRAM_AP_POSITION                                SOC_PETRA_DRAM_AP_POSITION;

#define SOC_PETRA_DRAM_BURST_SIZE_16                            SOC_TMC_DRAM_BURST_SIZE_16
#define SOC_PETRA_DRAM_BURST_SIZE_32                            SOC_TMC_DRAM_BURST_SIZE_32
#define SOC_PETRA_DRAM_NOF_BURST_SIZES                          SOC_TMC_DRAM_NOF_BURST_SIZES
typedef SOC_TMC_DRAM_BURST_SIZE                                 SOC_PETRA_DRAM_BURST_SIZE;

/* } */

/*************
 * MACROS    *
 *************/
/* { */

#define SOC_PETRA_DRAM_VAL_IN_CLOCKS(val)                 \
          (SOC_SAND_SET_FLD_IN_PLACE(val, SOC_PETRA_DRAM_VAL_SHIFT, SOC_PETRA_DRAM_VAL_MASK)) | \
          (SOC_SAND_SET_FLD_IN_PLACE(0x1, SOC_PETRA_DRAM_VAL_IS_IN_CLOCKS_SHIFT, SOC_PETRA_DRAM_VAL_IS_IN_CLOCKS_MASK))

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
  /*
   *  In this mode, the pattern is already configured to get a
   *  bottom threshold on the window size (worst case).
   */
  SOC_PETRA_DRAM_PATTERN_MODE_AUTO = 0,
  /*
   *  In this mode, the pattern is configurable in the 'data'
   *  field.
   */
  SOC_PETRA_DRAM_PATTERN_MODE_USER_DEF = 1,
  /*
   *  Number of types in SOC_PETRA_DRAM_PATTERN_MODE
   */
  SOC_PETRA_DRAM_NOF_PATTERN_MODES = 2
}SOC_PETRA_DRAM_PATTERN_MODE;

typedef union
{
  struct
  {
    SOC_SAND_MAGIC_NUM_VAR
    /*
     *  DDR2 - MRS0 (1st write)
     */
    uint32 mrs0_wr1;
    /*
     *  DDR2 - MRS0 (2nd write)
     */
    uint32 mrs0_wr2;
    /*
     *  DDR2 - EMR0 (1st write)
     */
    uint32 emr0_wr1;
    /*
     *  DDR2 - EMR0 (2nd write)
     */
    uint32 emr0_wr2;
    /*
     *  DDR2 - EMR0 (3rd write)
     */
    uint32 emr0_wr3;
    /*
     *  DDR2 - EMR1 (1st write)
     */
    uint32 emr1_wr1;
    /*
     *  DDR2 - EMR2 (1st write)
     */
    uint32 emr2_wr1;

  }ddr2;

  struct
  {
    SOC_SAND_MAGIC_NUM_VAR
    /*
     *  DDR3 - MRS0 (1st write)
     */
    uint32 mrs0_wr1;
    /*
     *  DDR3 - MRS0 (2nd write)
     */
    uint32 mrs0_wr2;
    /*
     *  DDR3 - MRS1 (1st write)
     */
    uint32 mrs1_wr1;
    /*
     *  DDR3 - MRS2 (1st write)
     */
    uint32 mrs2_wr1;
    /*
     *  DDR3 - MRS3 (1st write)
     */
    uint32 mrs3_wr1;

  }ddr3;

  struct
  {
    SOC_SAND_MAGIC_NUM_VAR
    /*
     *  GDD3 - MRS0 (1st write)
     */
    uint32 mrs0_wr1;
    /*
     *  GDD3 - EMR0 (1st write)
     */
    uint32 emr0_wr1;

  }gdd3;

} SOC_PETRA_DRAM_MODE_REGS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  T - Automatically calculate the mode registers (according to JEDEK)
   *  F - Use the values specified in 'mode_regs' for the mode registers
   */
  uint8 auto_mode;
  /*
   *  Number of Banks
   */
  SOC_PETRA_DRAM_NUM_BANKS nof_banks;
  /*
   *  Number of columns
   */
  SOC_PETRA_DRAM_NUM_COLUMNS nof_cols;
  /*
   *  Auto precharge bit position. Determines the position of the Auto
   *  Precharge bit in the address going to the DRAM
   */
  SOC_PETRA_DRAM_AP_POSITION ap_bit_pos;
  /*
   *  Dram burst size. May be 16 or 32 bytes. Must be set
   *  according to the dram's burst size
   */
  SOC_PETRA_DRAM_BURST_SIZE burst_size;
  /*
   *  Column Address Strobe latency. The period (clocks) between
   *  READ command and valid read data presented on the data
   *  out pins of the dram
   */
  uint32 c_cas_latency;
  /*
   *  The period (clocks) between write command and write data set
   *  on the dram data in pins
   */
  uint32 c_wr_latency;
  /*
   *  Refresh Cycle. Period between the active to the
   *  active/auto refresh commands (tRC)
   */
  uint32 t_rc;
  /*
   *  Row Refresh Cycle. Auto refresh command period. The
   *  minimal period between the refresh command and the
   *  next active command (tRFC)
   *  By default this period is stated in terms of picoseconds. To state
   *  it in terms of number of clocks use the macro SOC_PETRA_DRAM_VAL_IN_CLOCKS
   */
  uint32 t_rfc;
  /*
   *  Row Address Strobe. The minimal period needed to
   *  access a certain row of data in RAM between the data
   *  request and the precharge command (tRAS)
   *  By default this period is stated in terms of picoseconds. To state
   *  it in terms of number of clocks use the macro SOC_PETRA_DRAM_VAL_IN_CLOCKS
   */
  uint32 t_ras;
  /*
   *  Four Active Window. No more than four banks may be
   *  activated in a rolling window (tFAW)
   *  By default this period is stated in terms of picoseconds. To state
   *  it in terms of number of clocks use the macro SOC_PETRA_DRAM_VAL_IN_CLOCKS
   */
  uint32 t_faw;
  /*
   *  Row address to Column address Delay. The minimal period
   *  needed between RAS and CAS. It is the time required
   *  between row activation and read access to the column of
   *  the given memory block (tRcdRd)
   *  By default this period is stated in terms of picoseconds. To state
   *  it in terms of number of clocks use the macro SOC_PETRA_DRAM_VAL_IN_CLOCKS
   */
  uint32 t_rcd_rd;
  /*
   *  Row address to Column address Delay. The minimal period
   *  needed between RAS and CAS. It is the time required
   *  between row activation and write access to the column of
   *  the given memory block (tRcdWr)
   *  By default this period is stated in terms of picoseconds. To state
   *  it in terms of number of clocks use the macro SOC_PETRA_DRAM_VAL_IN_CLOCKS
   */
  uint32 t_rcd_wr;
  /*
   *  RAS To RAS delay. Active bank a to active bank command (tRRD)
   *  By default this period is stated in terms of picoseconds. To state
   *  it in terms of number of clocks use the macro SOC_PETRA_DRAM_VAL_IN_CLOCKS
   */
  uint32 t_rrd;
  /*
   *  Row Precharge. The minimal period between pre-charge
   *  action of a certain Row and the next consecutive action
   *  to the same bank/row (tRP)
   *  By default this period is stated in terms of picoseconds. To state
   *  it in terms of number of clocks use the macro SOC_PETRA_DRAM_VAL_IN_CLOCKS
   */
  uint32 t_rp;
  /*
   *  Write Recovery Time. Specifies the period that must
   *  elapse after the completion of a valid write operation,
   *  before a pre-charge command can be issued (tWR)
   *  By default this period is stated in terms of picoseconds. To state
   *  it in terms of number of clocks use the macro SOC_PETRA_DRAM_VAL_IN_CLOCKS
   */
  uint32 t_wr;
  /*
   *  Average periodic refresh interval.
   *  By default this period is stated in terms of picoseconds. To state
   *  it in terms of number of clocks use the macro SOC_PETRA_DRAM_VAL_IN_CLOCKS
   *  The value 0 disables the auto refresh mechanism.
   */
  uint32 t_ref;
  /*
   *  Write To Read Delay. The minimal period that must
   *  elapse between the last valid write operation and the
   *  next read command to the same internal bank of the DDR
   *  device (tWTR)
   *  By default this period is stated in terms of picoseconds. To state
   *  it in terms of number of clocks use the macro SOC_PETRA_DRAM_VAL_IN_CLOCKS
   */
  uint32 t_wtr;
  /*
   *  Read To Precharge Delay (tRTP)
   *  By default this period is stated in terms of picoseconds. To state
   *  it in terms of number of clocks use the macro SOC_PETRA_DRAM_VAL_IN_CLOCKS
   */
  uint32 t_rtp;
  /*
   *  Mode registers configuration
   */
  SOC_PETRA_DRAM_MODE_REGS_INFO mode_regs;

} SOC_PETRA_DRAM_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Mode of the pattern used for the BIST commands:
   *  pre-configured or user-specified.
   */
  SOC_PETRA_DRAM_PATTERN_MODE mode;
  /*
   *  Data of the pattern. Valid ony if the mode is
   *  user-specified. The two last words must be different for
   *  the pattern to be valid.
   */
  uint32 data[SOC_PETRA_DRAM_PATTERN_SIZE_IN_UINT32S];

} SOC_PETRA_DRAM_PATTERN;

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
   *  Ration between the window size and the (total) half
   *  cycle size. Represents an indication of the board
   *  quality. Type: percentage. Expected ratio: between 50%
   *  and 70%.
   */
  uint32 ratio_percent;

} SOC_PETRA_DRAM_WINDOW_INFO;

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

/*********************************************************************
* NAME:
*   soc_petra_dram_info_set
* TYPE:
*   PROC
* FUNCTION:
*   This function configures the dram according to the
*   provided sets of parameters. This function is called
*   during the initialization sequence and must not be
*   called afterwards
* INPUT:
*   SOC_SAND_IN  int                 unit -
*     Identifier of the device to access
*   SOC_SAND_IN  uint32                 dram_ndx -
*     Dram index. Range: 0-5
*   SOC_SAND_IN  uint32                 dram_freq -
*     Dram frequency (MHz)
*   SOC_SAND_IN  SOC_PETRA_DRAM_TYPE           dram_type -
*     One of three supported dram types (DDR2, DDR3, GDDR3)
*   SOC_SAND_IN  SOC_PETRA_DRAM_INFO           *info -
*     Dram configuration information
* REMARKS:
*   None
* RETURNS:
*   OK or ERROR indication
*********************************************************************/
uint32
  soc_petra_dram_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  SOC_PETRA_DRAM_TYPE           dram_type,
    SOC_SAND_IN  SOC_PETRA_DRAM_INFO           *info
  );

/*********************************************************************
* NAME:
*   soc_petra_dram_info_get
* TYPE:
*   PROC
* FUNCTION:
*   This function configures the dram according to the
*   provided sets of parameters. This function is called
*   during the initialization sequence and must not be
*   called afterwards
* INPUT:
*   SOC_SAND_IN  int                 unit -
*     Identifier of the device to access
*   SOC_SAND_IN  uint32                 dram_ndx -
*     Dram index. Range: 0-5
*   SOC_SAND_IN  uint32                 dram_freq -
*     Dram frequency (MHz)
*   SOC_SAND_OUT SOC_PETRA_DRAM_TYPE           *dram_type -
*     One of three supported dram types (DDR2, DDR3, GDDR3)
*   SOC_SAND_OUT SOC_PETRA_DRAM_INFO           *info -
*     Dram configuration information
* REMARKS:
*   None
* RETURNS:
*   OK or ERROR indication
*********************************************************************/
uint32
  soc_petra_dram_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_OUT SOC_PETRA_DRAM_TYPE           *dram_type,
    SOC_SAND_OUT SOC_PETRA_DRAM_INFO           *info
  );

/*********************************************************************
* NAME:
 *   soc_petra_dram_window_validity_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Diagnostic tool to get the DRAM valid window size
 *   through BIST commands. .
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 dram_ndx -
 *     Dram index. Range: 0 - 5.
 *   SOC_SAND_IN  SOC_PETRA_DRAM_PATTERN                        *pattern -
 *     Pattern used for the BIST commands.
 *   SOC_SAND_OUT SOC_PETRA_DRAM_WINDOW_INFO                    *window_info -
 *     Parameters characterizing the DRAM window validity.
 * REMARKS:
 *   1. This API is used for diagnostic only. NO traffic can
 *   go through the device after the call to this API and no
 *   other API must be called after this API.2. The expected
 *   ratio is between 50% and 70%.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_dram_window_validity_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DRAM_PATTERN                        *pattern,
    SOC_SAND_OUT SOC_PETRA_DRAM_WINDOW_INFO                    *window_info
  );

void
  soc_petra_PETRA_DRAM_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DRAM_INFO *info
  );

void
  soc_petra_PETRA_DRAM_PATTERN_clear(
    SOC_SAND_OUT SOC_PETRA_DRAM_PATTERN *info
  );

void
  soc_petra_PETRA_DRAM_WINDOW_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_DRAM_WINDOW_INFO *info
  );

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_DRAM_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_DRAM_TYPE enum_val
  );

const char*
  soc_petra_PETRA_DRAM_BURST_SIZE_to_string(
    SOC_SAND_IN SOC_PETRA_DRAM_BURST_SIZE enum_val
  );

const char*
  soc_petra_PETRA_DRAM_NUM_BANKS_to_string(
    SOC_SAND_IN SOC_PETRA_DRAM_NUM_BANKS enum_val
  );

const char*
  soc_petra_PETRA_DRAM_NUM_COLUMNS_to_string(
    SOC_SAND_IN SOC_PETRA_DRAM_NUM_COLUMNS enum_val
  );

const char*
  soc_petra_PETRA_DRAM_AP_POSITION_to_string(
    SOC_SAND_IN SOC_PETRA_DRAM_AP_POSITION enum_val
  );

const char*
  soc_petra_PETRA_DRAM_PATTERN_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_DRAM_PATTERN_MODE enum_val
  );

void
  soc_petra_PETRA_DRAM_MR_INFO_print(
    SOC_SAND_IN SOC_PETRA_DRAM_TYPE           dram_type,
    SOC_SAND_IN SOC_PETRA_DRAM_MODE_REGS_INFO *info
  );

void
  soc_petra_PETRA_DRAM_INFO_print(
    SOC_SAND_IN SOC_PETRA_DRAM_TYPE dram_type,
    SOC_SAND_IN SOC_PETRA_DRAM_INFO *info
  );

void
  soc_petra_PETRA_DRAM_PATTERN_print(
    SOC_SAND_IN  SOC_PETRA_DRAM_PATTERN *info
  );

void
  soc_petra_PETRA_DRAM_WINDOW_INFO_print(
    SOC_SAND_IN  SOC_PETRA_DRAM_WINDOW_INFO *info
  );

uint32
  soc_petra_dram_pckt_from_buff_read(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  buff_id,
    SOC_SAND_IN uint32  pckt_size_in_buff
  );
#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */
#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_API_DRAM_INCLUDED__*/
#endif
