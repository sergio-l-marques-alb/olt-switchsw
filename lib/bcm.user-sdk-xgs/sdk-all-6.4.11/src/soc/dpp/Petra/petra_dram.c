/* $Id: petra_dram.c,v 1.10 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>


#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_api_mgmt.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_dram.h>
#include <soc/dpp/Petra/petra_api_diagnostics.h>
#include <soc/dpp/Petra/petra_diagnostics.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_init.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_debug.h>
#include <soc/dpp/Petra/petra_bitstream.h>
#include <soc/dpp/Petra/petra_ingress_traffic_mgmt.h>

#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_init.h>
#endif
/* } */

/*************
 * DEFINES   *
 *************/
/* { */
/* $Id: petra_dram.c,v 1.10 Broadcom SDK $
 *  DRAM window validity constants
 */
#define SOC_PETRA_DRAM_NOF_TAPS                                    (64)
#define SOC_PETRA_DRAM_RAM_VAL_IN_UINT32S                          (2)
#define SOC_PETRA_DRAM_RAM_NOF_FLDS                                (4)
#define SOC_PETRA_DRAM_NOF_SHIFT_VAL_REGS                          (4)
#define SOC_PETRA_DRAM_SHIFT_VAL                                   (0x88888888)

#define SOC_PETRA_DRAM_PATTERN_VAL_0                               (0xFFFFFFFE)
#define SOC_PETRA_DRAM_PATTERN_VAL_1                               (0x00000001)
#define SOC_PETRA_DRAM_PATTERN_VAL_2                               (0xFFEFFFFF)
#define SOC_PETRA_DRAM_PATTERN_VAL_3                               (0x40000000)
#define SOC_PETRA_DRAM_PATTERN_VAL_4                               (0xDFFFFFEF)
#define SOC_PETRA_DRAM_PATTERN_VAL_5                               (0x00010000)
#define SOC_PETRA_DRAM_PATTERN_VAL_6                               (0xFFFFFFFF)
#define SOC_PETRA_DRAM_PATTERN_VAL_7                               (0x00000000)

#define SOC_PETRA_DRAM_NOF_HALF_CYCLE_REGS                         (4)
#define SOC_PETRA_DRAM_DLL_CONTROL_VAL                             (0xf802)
#define SOC_PETRA_DRAM_RAM_ENTRY_OFFSET                            (0x40)
#define SOC_PETRA_DRAM_SHIFT_NOF_TAPS                              (8)
#define SOC_PETRA_DRAM_PERCENT_FACTOR                              (100)
#define SOC_PETRA_DRAM_MSB_MASK                                    (0x80000000)


/* DRAM Window Validity */
#define SOC_PETRA_DRAM_WINDOW_VALIDITY_BUSY_ITERS   50
#define SOC_PETRA_DRAM_WINDOW_VALIDITY_TIMER_ITERS  5
#define SOC_PETRA_DRAM_WINDOW_VALIDITY_TIMER_DELAY  16


/* MR0 MRS */
#define SOC_PETRA_DRAM_MR_MRS_LSB         (14)
#define SOC_PETRA_DRAM_MR_MRS_MSB         (15)
#define SOC_PETRA_DRAM_MR0_MRS            ( 0)
#define SOC_PETRA_DRAM_MR1_MRS            ( 1)
#define SOC_PETRA_DRAM_MR2_MRS            ( 2)
#define SOC_PETRA_DRAM_MR3_MRS            ( 3)

/* MR0 DLL reset */
#define SOC_PETRA_DRAM_MR0_DLL_LSB        ( 8)
#define SOC_PETRA_DRAM_MR0_DLL_MSB        ( 8)

/* MR0 burst type */
#define SOC_PETRA_DRAM_MR0_BT             ( 0)

/* DDR2 { */
#define SOC_PETRA_DRAM_EMR1_OCD_LSB       ( 7)
#define SOC_PETRA_DRAM_EMR1_OCD_MSB       ( 9)

#define SOC_PETRA_DRAM_DDR2_CL_MIN        ( 3)
#define SOC_PETRA_DRAM_DDR2_CL_MAX        ( 7)

#define SOC_PETRA_DRAM_DDR2_WR_MIN        ( 2)
#define SOC_PETRA_DRAM_DDR2_WR_MAX        ( 8)

/* MR0 { */
/* burst length */
#define SOC_PETRA_DRAM_DDR2_MR0_BL_16     ( 2)
#define SOC_PETRA_DRAM_DDR2_MR0_BL_32     ( 3)
/* Precharge PD */
#define SOC_PETRA_DRAM_DDR2_MR0_PD        ( 0)
/* Test Mode TM */
#define SOC_PETRA_DRAM_DDR2_MR0_TM        ( 0)
/* } MR0 */
/* MR1 { */
/* DLL enable */
#define SOC_PETRA_DRAM_DDR2_MR1_DLL       ( 0)
/* Output Drive Strength */
#define SOC_PETRA_DRAM_DDR2_MR1_ODS       ( 1)
/* RTT #0 */
#define SOC_PETRA_DRAM_DDR2_MR1_RTT0      ( 0)
/* Additive Latency (AL) */
#define SOC_PETRA_DRAM_DDR2_MR1_AL        ( 0)
/* RTT #1 */
#define SOC_PETRA_DRAM_DDR2_MR1_RTT1      ( 1)
/* OCD Operation */
#define SOC_PETRA_DRAM_DDR2_MR1_OCD       ( 0)
/* DQS enable */
#define SOC_PETRA_DRAM_DDR2_MR1_DQS       ( 0)
/* RQDS enable */
#define SOC_PETRA_DRAM_DDR2_MR1_RDQS      ( 0)
/* Outputs */
#define SOC_PETRA_DRAM_DDR2_MR1_OUT       ( 0)
/* } MR1 */
/* MR2 { */
/*  Self Refresh Temperature */
/* } MR0 */
/* } DDR2 */

/* DDR3 { */
#define SOC_PETRA_DRAM_DDR3_WL_MIN        ( 5)
#define SOC_PETRA_DRAM_DDR3_WL_MAX        ( 8)

#define SOC_PETRA_DRAM_DDR3_CL_MIN        ( 5)
#define SOC_PETRA_DRAM_DDR3_CL_MAX        (11)

#define SOC_PETRA_DRAM_DDR3_WR_MIN        ( 5)
#define SOC_PETRA_DRAM_DDR3_WR_MAX        (12)

/* MR0 { */
/* Burst Length */
#define SOC_PETRA_DRAM_DDR3_MR0_BL_16     ( 2)
#define SOC_PETRA_DRAM_DDR3_MR0_BL_32     ( 0)
/* Precharge PD */
#define SOC_PETRA_DRAM_DDR3_MR0_PD        ( 0)
/* } MR0 */
/* MR1 { */
/* MR1 DLL enable */
#define SOC_PETRA_DRAM_DDR3_MR1_DLL       ( 0)
/* MR1 Output Drive Strength #0 */
#define SOC_PETRA_DRAM_DDR3_MR1_ODS0      ( 1)
/* MR1 RTT #0 */
#define SOC_PETRA_DRAM_DDR3_MR1_RTT0      ( 0)
/* MR1 Additive Latency (AL) */
#define SOC_PETRA_DRAM_DDR3_MR1_AL        ( 0)
/* MR1 Output Drive Strength #1 */
#define SOC_PETRA_DRAM_DDR3_MR1_ODS1      ( 0)
/* MR1 RTT #1 */
#define SOC_PETRA_DRAM_DDR3_MR1_RTT1      ( 1)
/* MR1 Write Levelization (WL) */
#define SOC_PETRA_DRAM_DDR3_MR1_WL        ( 0)
/* MR1 RTT #2 */
#define SOC_PETRA_DRAM_DDR3_MR1_RTT2      ( 0)
/* MR1 TQDS enable */
#define SOC_PETRA_DRAM_DDR3_MR1_TQDS      ( 0)
/* MR1 Q Off */
#define SOC_PETRA_DRAM_DDR3_MR1_QOFF      ( 0)
/* } MR1 */
/* MR2 { */
/*  Auto Self Refresh */
#define SOC_PETRA_DRAM_DDR3_MR2_ASR       ( 0)
/*  Self Refresh Temperature */
#define SOC_PETRA_DRAM_DDR3_MR2_SRT       ( 0)
/*  Dynamic ODT */
#define SOC_PETRA_DRAM_DDR3_MR2_RTT       ( 0)
/* } MR2 */
/* } DDR3 */

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

#define SOC_PETRA_DRAM_VAL(val)                                 \
          (SOC_SAND_GET_FLD_FROM_PLACE(val, SOC_PETRA_DRAM_VAL_SHIFT, SOC_PETRA_DRAM_VAL_MASK))

#define SOC_PETRA_DRAM_IS_VAL_IN_CLOCKS(val)                    \
          (SOC_SAND_GET_FLD_FROM_PLACE(val, SOC_PETRA_DRAM_VAL_IS_IN_CLOCKS_SHIFT, SOC_PETRA_DRAM_VAL_IS_IN_CLOCKS_MASK))

#define SOC_PETRA_DRAM_VAL_TO_CLOCKS(value, round_up)                     \
          (SOC_PETRA_DRAM_IS_VAL_IN_CLOCKS(value) ? (SOC_PETRA_DRAM_VAL(value)) : soc_petra_64bit_dram_clocks_to_val(round_up,dram_freq, SOC_PETRA_DRAM_VAL(value), 1000000))



#define SOC_PETRA_DRAM_FLD_IN_PLACE_IF_EQ(val, eq, set, fld)    \
          SOC_PETRA_FLD_IN_PLACE(( ((val) == (eq)) ? set : 0x0), fld)

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef struct
{
  /*
   *  0x1801, 0x1a01, 0x1c01, 0x1e01, 0x2001, 0x2201
   *  DDR2 - MRS1 (1st write)
   *  DDR3 - MRS1 (1st write)
   *  GDD3 - MRS1 (1st write)
   */
  uint32 mr0;
  /*
   *  0x1802, 0x1a02, 0x1c02, 0x1e02, 0x2002, 0x2202
   *  DDR2 - MRS1 (2nd write)
   *  DDR3 - MRS1 (2nd write)
   *  GDD3 - Don't care
   */
  uint32 mr1;
  /*
   *  0x1803, 0x1a03, 0x1c03, 0x1e03, 0x2003, 0x2203
   *  DDR2 - EMR1 (1st write)
   *  DDR3 - EMR1 (1st write)
   *  GDD3 - EMR1 (1st write)
   */
  uint32 mr2;
  /*
   *  0x1816, 0x1a16, 0x1c16, 0x1e16, 0x2016, 0x2216
   *  DDR2 - EMR1 (2nd write)
   *  DDR3 - Don't care
   *  GDD3 - Don't care
   */
  uint32 mr3;
  /*
   *  0x1817, 0x1a17, 0x1c17, 0x1e17, 0x2017, 0x2217
   *  DDR2 - EMR1 (3rd write)
   *  DDR3 - Don't care
   *  GDD3 - Don't care
   */
  uint32 mr4;
  /*
   *  0x1818, 0x1a18, 0x1c18, 0x1e18, 0x2018, 0x2218
   *  DDR2 - EMR2 (1st write)
   *  DDR3 - MRS2 (1st write)
   *  GDD3 - Don't care
   */
  uint32 mr5;
  /*
   *  0x1819, 0x1a19, 0x1c19, 0x1e19, 0x2019, 0x2219
   *  DDR2 - EMR3 (1st write)
   *  DDR3 - MRS3 (1st write)
   *  GDD3 - Don't care
   */
  uint32 mr6;

} SOC_PETRA_DRAM_INTERNAL_MR_INFO;

typedef struct
{
  /*
   *  Dram burst size. May be 16 or 32 bytes. Must be set
   *  according to the dram's burst size
   */
  SOC_PETRA_DRAM_BURST_SIZE burst_size;
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
   *  Number of Banks
   */
  SOC_PETRA_DRAM_NUM_BANKS nof_banks;
  /*
   *  Column Address Strobe latency. The period (clocks) between
   *  READ command and valid read data presented on the data
   *  out pins of the dram.
   */
  uint32 c_cas_latency;
  /*
   *  The period (clocks) between WRITE command and write data set
   *  on the dram data in pins.
   */
  uint32 c_wr_latency;
  /*
   *  Refresh Cycle. Period (clocks) between the active to the
   *  active/auto refresh commands
   */
  uint32 c_rc;
  /*
   *  Row Refresh Cycle. Auto refresh command period. The
   *  minimal period (clocks) between the refresh command and the
   *  next active command
   */
  uint32 c_rfc;
  /*
   *  Row Address Strobe. The minimal period (clocks) needed to
   *  access a certain row of data in RAM between the data
   *  request and the precharge command
   */
  uint32 c_ras;
  /*
   *  Four Active Window. No more than four banks may be
   *  activated in a rolling window
   */
  uint32 c_faw;
  /*
   *  Row address to Column address Delay. The minimal period
   *  (clocks) needed between RAS and CAS. It is the time required
   *  between row activation and read access to the column of
   *  the given memory block.
   */
  uint32 c_rcd_rd;
  /*
   *  Row address to Column address Delay. The minimal period
   *  (clocks) needed between RAS and CAS. It is the time required
   *  between row activation and write access to the column of
   *  the given memory block.
   */
  uint32 c_rcd_wr;
  /*
   *  RAS To RAS delay. Active bank a to active bank command
   */
  uint32 c_rrd;
  /*
   *  Row Precharge. The minimal period between pre-charge
   *  action of a certain Row and the next consecutive action
   *  to the same bank/row.
   */
  uint32 c_rp;
  /*
   *  Write Recovery Time. Specifies the period (clocks) that must
   *  elapse after the completion of a valid write operation,
   *  before a pre-charge command can be issued.
   */
  uint32 c_wr;
  /*
   *  Write To Read Delay. The minimal period (clocks) that must
   *  elapse between the last valid write operation and the
   *  next read command to the same internal bank of the DDR
   *  device.
   */
  uint32 c_wtr;
  /*
  *   Average periodic refresh interval, in units of 32 clocks. The value 0
  *   disables the auto refresh mechanism.
   */
  uint32 c_ref;
  /*
   *  Read To Precharge Delay
   */
  uint32 c_rtp;
  /*
   *  The period (clocks) that must elapse between stable power
   *  value and the dram reset out (usually asserting CKE).
   */
  uint32 c_rst;
  /*
   *  Layout-specific timing correction values poked into them
   *  to allow for the different trace lengths to different
   *  data bus pins of the RAM modules. Should be '1'.
   */
  uint32 c_pcb;
  /*
   *  Period (clocks) from active command to a read command with auto
   *  precharge
   */
  uint32 c_ras_rd;
  /*
   *  Period (clocks) from active command to a write command with auto
   *  precharge
   */
  uint32 c_ras_wr;
  /*
   *  Period (clocks) from a read command with auto precharge until the
   *  precharge command period is over
   */
  uint32 c_rd_ap;
  /*
   *  Period (clocks) from a write command with auto precharge until
   *  the precharge command period is over
   */
  uint32 c_wr_ap;
  /*
   *  Period (clocks) between a write command and a read command
   */
  uint32 c_wr_rd;
  /*
   *  Period (clocks) between the end of a read burst and a write
   *  command. Does not include the four cycles of the read
   *  burst
   */
  uint32 c_rd_wr;

} SOC_PETRA_DRAM_INTERNAL_INFO;

typedef struct
{
  uint8      msb;
  uint8      lsb;
}  SOC_PETRA_DRAM_MR_FIELD;

typedef struct
{
  struct
  {
    /* MR0 burst length */
    SOC_PETRA_DRAM_MR_FIELD bl;
    /* MR0 burst type */
    SOC_PETRA_DRAM_MR_FIELD bt;
    /* MR0 CAS latency */
    SOC_PETRA_DRAM_MR_FIELD cl;
    /* MR0 Test Mode */
    SOC_PETRA_DRAM_MR_FIELD tm;
    /* MR0 Write Recovery */
    SOC_PETRA_DRAM_MR_FIELD wr;
    /* MR0 Precharge PD */
    SOC_PETRA_DRAM_MR_FIELD pd;

  } mr0;

  struct
  {
    /* MR1 DLL enable */
    SOC_PETRA_DRAM_MR_FIELD dll;
    /* MR1 Output Drive Strength */
    SOC_PETRA_DRAM_MR_FIELD ods;
    /* MR1 RTT #0 */
    SOC_PETRA_DRAM_MR_FIELD rtt0;
    /* MR1 Additive Latency (AL) */
    SOC_PETRA_DRAM_MR_FIELD al;
    /* MR1 RTT #1 */
    SOC_PETRA_DRAM_MR_FIELD rtt1;
    /* MR1 OCD Operation */
    SOC_PETRA_DRAM_MR_FIELD ocd;
    /* MR1 DQS enable */
    SOC_PETRA_DRAM_MR_FIELD dqs;
    /* MR1 RQDS enable */
    SOC_PETRA_DRAM_MR_FIELD rdqs;
    /* MR1 Outputs */
    SOC_PETRA_DRAM_MR_FIELD out;

  } mr1;

  struct
  {
    /* MR2 Self Refresh Temperature */
    SOC_PETRA_DRAM_MR_FIELD srt;

  } mr2;

} SOC_PETRA_DRAM_DDR2_MODE_REGS;

typedef struct
{
  struct
  {
    /* MR0 burst length */
    SOC_PETRA_DRAM_MR_FIELD bl;
    /* MR0 burst type */
    SOC_PETRA_DRAM_MR_FIELD bt;
    /* MR0 CAS latency */
    SOC_PETRA_DRAM_MR_FIELD cl;
    /* MR0 Write Recovery */
    SOC_PETRA_DRAM_MR_FIELD wr;
    /* MR0 Precharge PD */
    SOC_PETRA_DRAM_MR_FIELD pd;

  } mr0;

  struct
  {
    /* MR1 DLL enable */
    SOC_PETRA_DRAM_MR_FIELD dll;
    /* MR1 Output Drive Strength #0 */
    SOC_PETRA_DRAM_MR_FIELD ods0;
    /* MR1 RTT #0 */
    SOC_PETRA_DRAM_MR_FIELD rtt0;
    /* MR1 Additive Latency (AL) */
    SOC_PETRA_DRAM_MR_FIELD al;
    /* MR1 Output Drive Strength #1 */
    SOC_PETRA_DRAM_MR_FIELD ods1;
    /* MR1 RTT #1 */
    SOC_PETRA_DRAM_MR_FIELD rtt1;
    /* MR1 Write Levelization (WL) */
    SOC_PETRA_DRAM_MR_FIELD wl;
    /* MR1 RTT #2 */
    SOC_PETRA_DRAM_MR_FIELD rtt2;
    /* MR1 TQDS enable */
    SOC_PETRA_DRAM_MR_FIELD tqds;
    /* MR1 Q Off */
    SOC_PETRA_DRAM_MR_FIELD qoff;

  } mr1;

  struct
  {
    /* MR2 CAS Write Latency */
    SOC_PETRA_DRAM_MR_FIELD cwl;
    /* MR2 Auto Self Refresh */
    SOC_PETRA_DRAM_MR_FIELD asr;
    /* MR2 Self Refresh Temperature */
    SOC_PETRA_DRAM_MR_FIELD srt;
    /* MR2 Dynamic ODT */
    SOC_PETRA_DRAM_MR_FIELD rtt;

  } mr2;

} SOC_PETRA_DRAM_DDR3_MODE_REGS;

typedef union
{
  /*
   *  DDR2 mode registers t_info
   */
  SOC_PETRA_DRAM_DDR2_MODE_REGS ddr2;
  /*
   *  DDR3 mode registers t_info
   */
  SOC_PETRA_DRAM_DDR3_MODE_REGS ddr3;

} SOC_PETRA_DRAM_MODE_REGS;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */


/* } */
/*************
 * FUNCTIONS PROTOTYPES*
 *************/

/*********************************************************************
*  This function calculate clock value will be written to the clock register
*  Calculation is done in 64 bit resolution and in round down
*  
*********************************************************************/
STATIC uint32
  soc_petra_64bit_dram_clocks_to_val(
   SOC_SAND_IN  uint8           is_round_up,
   SOC_SAND_IN  uint32           value,
    SOC_SAND_IN  uint32           mult2,
   SOC_SAND_IN  uint32           dram_freq
  );

/*************
 * FUNCTIONS *
 *************/
/* { */

STATIC void
  soc_petra_mode_reg_fld_set(
    SOC_SAND_OUT SOC_PETRA_DRAM_MR_FIELD       *field,
    SOC_SAND_IN  uint8                   msb,
    SOC_SAND_IN  uint8                   lsb
  )
{
   field->msb  = msb;
   field->lsb  = lsb;
}

/*********************************************************************
*  Mode register formatting
*********************************************************************/
STATIC uint32
  soc_petra_mode_regs(
    SOC_SAND_IN  SOC_PETRA_DRAM_TYPE           dram_type,
    SOC_SAND_OUT SOC_PETRA_DRAM_MODE_REGS      *mode_regs
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(mode_regs);

  if (dram_type == SOC_PETRA_DRAM_TYPE_DDR2)
  {
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr2.mr0.bl)  ,  2,  0);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr2.mr0.bt)  ,  3,  3);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr2.mr0.cl)  ,  6,  4);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr2.mr0.tm)  ,  7,  7);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr2.mr0.wr)  , 11,  9);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr2.mr0.pd)  , 12, 12);

    soc_petra_mode_reg_fld_set( &(mode_regs->ddr2.mr1.dll) ,  0,  0);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr2.mr1.ods) ,  1,  1);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr2.mr1.rtt0),  2,  2);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr2.mr1.al)  ,  5,  3);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr2.mr1.rtt1),  6,  6);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr2.mr1.ocd) ,  9,  7);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr2.mr1.dqs) , 10, 10);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr2.mr1.rdqs), 11, 11);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr2.mr1.out) , 12, 12);

    soc_petra_mode_reg_fld_set( &(mode_regs->ddr2.mr2.srt) ,  7,  7);
  }
  else if (dram_type == SOC_PETRA_DRAM_TYPE_DDR3)
  {
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr0.bl)  ,  1,  0);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr0.bt)  ,  3,  3);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr0.cl)  ,  6,  4);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr0.wr)  , 11,  9);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr0.pd)  , 12, 12);

    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr1.dll) ,  0,  0);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr1.ods0),  1,  1);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr1.rtt0),  2,  2);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr1.al)  ,  4,  3);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr1.ods1),  5,  5);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr1.rtt1),  6,  6);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr1.wl)  ,  7,  7);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr1.rtt2),  9,  9);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr1.tqds), 11, 11);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr1.qoff), 12, 12);

    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr2.cwl) ,  5,  3);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr2.asr) ,  6,  6);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr2.srt) ,  7,  7);
    soc_petra_mode_reg_fld_set( &(mode_regs->ddr3.mr2.rtt) , 10,  9);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mode_regs()",0,0);
}

/*********************************************************************
*  This function gets the value of MR0 (DDR2)
*********************************************************************/
STATIC uint32
  soc_petra_dram_ddr2_mrs0_get(
    SOC_SAND_IN  SOC_PETRA_DRAM_DDR2_MODE_REGS *mode_regs,
    SOC_SAND_IN  SOC_PETRA_DRAM_INTERNAL_INFO  *c_info
  )
{
  uint32
    reg_val = 0;

  reg_val |= SOC_PETRA_DRAM_FLD_IN_PLACE_IF_EQ(c_info->burst_size, SOC_PETRA_DRAM_BURST_SIZE_16, SOC_PETRA_DRAM_DDR2_MR0_BL_16, mode_regs->mr0.bl);
  reg_val |= SOC_PETRA_DRAM_FLD_IN_PLACE_IF_EQ(c_info->burst_size, SOC_PETRA_DRAM_BURST_SIZE_32, SOC_PETRA_DRAM_DDR2_MR0_BL_32, mode_regs->mr0.bl);

  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_MR0_BT, mode_regs->mr0.bt);

  reg_val |= SOC_PETRA_FLD_IN_PLACE(c_info->c_cas_latency, mode_regs->mr0.cl);

  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR2_MR0_TM, mode_regs->mr0.tm);

  reg_val |= SOC_PETRA_FLD_IN_PLACE(c_info->c_wr - 1, mode_regs->mr0.wr);

  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR2_MR0_PD, mode_regs->mr0.pd);

  reg_val |= SOC_SAND_SET_BITS_RANGE(SOC_PETRA_DRAM_MR0_MRS, SOC_PETRA_DRAM_MR_MRS_MSB, SOC_PETRA_DRAM_MR_MRS_LSB);
  return reg_val;
}

/*********************************************************************
*  This function gets the value of MR1
*********************************************************************/
STATIC uint32
  soc_petra_dram_ddr2_emr0_get(
    SOC_SAND_IN  SOC_PETRA_DRAM_DDR2_MODE_REGS *mode_regs,
    SOC_SAND_IN  SOC_PETRA_DRAM_INTERNAL_INFO  *c_info
  )
{
  uint32
    reg_val = 0;

  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR2_MR1_DLL , mode_regs->mr1.dll );
  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR2_MR1_ODS , mode_regs->mr1.ods );
  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR2_MR1_RTT0, mode_regs->mr1.rtt0);
  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR2_MR1_AL  , mode_regs->mr1.al  );
  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR2_MR1_RTT1, mode_regs->mr1.rtt1);
  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR2_MR1_OCD , mode_regs->mr1.ocd );
  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR2_MR1_DQS , mode_regs->mr1.dqs );
  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR2_MR1_RDQS, mode_regs->mr1.rdqs);
  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR2_MR1_OUT , mode_regs->mr1.out );

  reg_val |= SOC_SAND_SET_BITS_RANGE(SOC_PETRA_DRAM_MR1_MRS, SOC_PETRA_DRAM_MR_MRS_MSB, SOC_PETRA_DRAM_MR_MRS_LSB);
  return reg_val;
}

/*********************************************************************
*  This function gets the value of MR2 (DDR2)
*********************************************************************/
STATIC uint32
  soc_petra_dram_ddr2_emr1_get(
    SOC_SAND_IN  SOC_PETRA_DRAM_DDR2_MODE_REGS *mode_regs,
    SOC_SAND_IN  SOC_PETRA_DRAM_INTERNAL_INFO  *c_info
  )
{
  uint32
    reg_val = 0;

  reg_val |= SOC_SAND_SET_BITS_RANGE(SOC_PETRA_DRAM_MR2_MRS, SOC_PETRA_DRAM_MR_MRS_MSB, SOC_PETRA_DRAM_MR_MRS_LSB);
  return reg_val;
  }

/*********************************************************************
*  This function gets the value of MR2 (DDR2)
*********************************************************************/
STATIC uint32
  soc_petra_dram_ddr2_emr2_get(
    SOC_SAND_IN  SOC_PETRA_DRAM_DDR2_MODE_REGS *mode_regs,
    SOC_SAND_IN  SOC_PETRA_DRAM_INTERNAL_INFO  *c_info
  )
  {
  uint32
    reg_val = 0;

  reg_val |= SOC_SAND_SET_BITS_RANGE(SOC_PETRA_DRAM_MR3_MRS, SOC_PETRA_DRAM_MR_MRS_MSB, SOC_PETRA_DRAM_MR_MRS_LSB);
  return reg_val;
}

/*********************************************************************
*  This function gets the value of MR0 (DDR3)
*********************************************************************/
STATIC uint32
  soc_petra_dram_ddr3_mrs0_get(
    SOC_SAND_IN  SOC_PETRA_DRAM_DDR3_MODE_REGS *mode_regs,
    SOC_SAND_IN  SOC_PETRA_DRAM_INTERNAL_INFO  *c_info
  )
{
  uint32
    reg_val = 0;

  reg_val |= SOC_PETRA_DRAM_FLD_IN_PLACE_IF_EQ(c_info->burst_size, SOC_PETRA_DRAM_BURST_SIZE_16, SOC_PETRA_DRAM_DDR3_MR0_BL_16, mode_regs->mr0.bl);
  reg_val |= SOC_PETRA_DRAM_FLD_IN_PLACE_IF_EQ(c_info->burst_size, SOC_PETRA_DRAM_BURST_SIZE_32, SOC_PETRA_DRAM_DDR3_MR0_BL_32, mode_regs->mr0.bl);

  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_MR0_BT, mode_regs->mr0.bt);

  reg_val |= SOC_PETRA_FLD_IN_PLACE(c_info->c_cas_latency - 4, mode_regs->mr0.cl);

  reg_val |= SOC_PETRA_DRAM_FLD_IN_PLACE_IF_EQ(c_info->c_wr,  5, 1, mode_regs->mr0.wr);
  reg_val |= SOC_PETRA_DRAM_FLD_IN_PLACE_IF_EQ(c_info->c_wr,  6, 2, mode_regs->mr0.wr);
  reg_val |= SOC_PETRA_DRAM_FLD_IN_PLACE_IF_EQ(c_info->c_wr,  7, 3, mode_regs->mr0.wr);
  reg_val |= SOC_PETRA_DRAM_FLD_IN_PLACE_IF_EQ(c_info->c_wr,  8, 4, mode_regs->mr0.wr);
  reg_val |= SOC_PETRA_DRAM_FLD_IN_PLACE_IF_EQ(c_info->c_wr, 10, 5, mode_regs->mr0.wr);
  reg_val |= SOC_PETRA_DRAM_FLD_IN_PLACE_IF_EQ(c_info->c_wr, 12, 6, mode_regs->mr0.wr);

  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR3_MR0_PD, mode_regs->mr0.pd);

  reg_val |= SOC_SAND_SET_BITS_RANGE(SOC_PETRA_DRAM_MR0_MRS, SOC_PETRA_DRAM_MR_MRS_MSB, SOC_PETRA_DRAM_MR_MRS_LSB);
  return reg_val;
}

/*********************************************************************
*  This function gets the value of MR1
*********************************************************************/
STATIC uint32
  soc_petra_dram_ddr3_mrs1_get(
    SOC_SAND_IN  SOC_PETRA_DRAM_DDR3_MODE_REGS *mode_regs,
    SOC_SAND_IN  SOC_PETRA_DRAM_INTERNAL_INFO  *c_info
  )
{
  uint32
    reg_val = 0;

  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR3_MR1_DLL , mode_regs->mr1.dll );
  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR3_MR1_ODS0, mode_regs->mr1.ods0);
  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR3_MR1_RTT0, mode_regs->mr1.rtt0);
  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR3_MR1_AL  , mode_regs->mr1.al  );
  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR3_MR1_ODS1, mode_regs->mr1.ods1);
  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR3_MR1_RTT1, mode_regs->mr1.rtt1);
  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR3_MR1_WL  , mode_regs->mr1.wl  );
  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR3_MR1_RTT2, mode_regs->mr1.rtt2);
  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR3_MR1_TQDS, mode_regs->mr1.tqds);
  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR3_MR1_QOFF, mode_regs->mr1.qoff);

  reg_val |= SOC_SAND_SET_BITS_RANGE(SOC_PETRA_DRAM_MR1_MRS, SOC_PETRA_DRAM_MR_MRS_MSB, SOC_PETRA_DRAM_MR_MRS_LSB);
  return reg_val;
}

/*********************************************************************
*  This function gets the value of MR2 (DDR3)
*********************************************************************/
STATIC uint32
  soc_petra_dram_ddr3_mrs2_get(
    SOC_SAND_IN  SOC_PETRA_DRAM_DDR3_MODE_REGS *mode_regs,
    SOC_SAND_IN  SOC_PETRA_DRAM_INTERNAL_INFO  *c_info
  )
{
  uint32
    reg_val = 0;

  reg_val |= SOC_PETRA_FLD_IN_PLACE(c_info->c_wr_latency - 5, mode_regs->mr2.cwl);

  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR3_MR2_ASR, mode_regs->mr2.asr);

  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR3_MR2_SRT, mode_regs->mr2.srt);

  reg_val |= SOC_PETRA_FLD_IN_PLACE(SOC_PETRA_DRAM_DDR3_MR2_RTT, mode_regs->mr2.rtt);

  reg_val |= SOC_SAND_SET_BITS_RANGE(SOC_PETRA_DRAM_MR2_MRS, SOC_PETRA_DRAM_MR_MRS_MSB, SOC_PETRA_DRAM_MR_MRS_LSB);
  return reg_val;
}

/*********************************************************************
*  This function gets the value of MR2 (DDR3)
*********************************************************************/
STATIC uint32
  soc_petra_dram_ddr3_mrs3_get(
    SOC_SAND_IN  SOC_PETRA_DRAM_DDR3_MODE_REGS *mode_regs,
    SOC_SAND_IN  SOC_PETRA_DRAM_INTERNAL_INFO  *c_info
  )
{
  uint32
    reg_val = 0;

  reg_val |= SOC_SAND_SET_BITS_RANGE(SOC_PETRA_DRAM_MR3_MRS, SOC_PETRA_DRAM_MR_MRS_MSB, SOC_PETRA_DRAM_MR_MRS_LSB);
  return reg_val;
}

STATIC uint32
  soc_petra_dram_internal_refresh_delay_period_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_freq
  )
{
  uint32
    value = SOC_SAND_DIV_ROUND_UP(20 * dram_freq, soc_petra_chip_mega_ticks_per_sec_get(unit));

  if (value < 40)
  {
    return 20;
  }
  if (value < 48)
  {
    return 30;
  }

  return 45;
}

/*********************************************************************
*  This function configures the dram according to the
*  provided sets of parameters
*  Details: in the H file. (search for prototype)
*********************************************************************/
STATIC uint32
  soc_petra_dram_info_set_internal_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  SOC_PETRA_DRAM_TYPE           dram_type,
    SOC_SAND_IN  SOC_PETRA_DRAM_INTERNAL_INFO    *c_info,
    SOC_SAND_IN  SOC_PETRA_DRAM_INTERNAL_MR_INFO *c_mr_info

  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    reg_val;
  uint32
    reg_idx;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DRAM_INFO_SET_UNSAFE);

  /* 0x1800  0x00000002 */
  reg_val = 0x00000002;
  SOC_PETRA_REG_ISET(regs->drc.ddr_controller_triggers_reg, reg_val, dram_ndx, 10, exit);

  /* 0x0c7f  0x80222222 */
  reg_val = 0x80222222;
  SOC_PETRA_REG_ISET(regs->dpi.io_config_reg, reg_val, dram_ndx, 12, exit);

  /* 0x1801  ---------- */
  reg_val = c_mr_info->mr0;
  SOC_PETRA_REG_ISET(regs->drc.ddr_mode_1_reg, reg_val, dram_ndx, 14, exit);

  /* 0x1802  ---------- */
  reg_val = c_mr_info->mr1;
  SOC_PETRA_REG_ISET(regs->drc.ddr_mode_2_reg, reg_val, dram_ndx, 16, exit);

  /* 0x1803  0x00004042 */
  reg_val = c_mr_info->mr2;
  SOC_PETRA_REG_ISET(regs->drc.ddr_extended_mode_1_reg, reg_val, dram_ndx, 18, exit);

  /* 0x1816  0x00004042 */
  reg_val = c_mr_info->mr3;
  SOC_PETRA_REG_ISET(regs->drc.extended_mode_wr2_reg, reg_val, dram_ndx, 20, exit);

  /* 0x1817  0x00004042 */
  reg_val = c_mr_info->mr4;
  SOC_PETRA_REG_ISET(regs->drc.ddr2_extended_mode_wr3_reg, reg_val, dram_ndx, 88, exit);

  /* 0x1818  0x0000c008 */
  reg_val = c_mr_info->mr5;;
  SOC_PETRA_REG_ISET(regs->drc.ddr_extended_mode_2_reg, reg_val, dram_ndx, 22, exit);

  /* 0x1819  0x00000000 */
  reg_val = c_mr_info->mr6;;
  SOC_PETRA_REG_ISET(regs->drc.ddr_extended_mode_3_reg, reg_val, dram_ndx, 23, exit);

  /* 0x1804  ---------- */
  reg_val = c_info->c_rst;
  SOC_PETRA_FLD_ISET(regs->drc.ac_operating_conditions1_reg.ddrt_rst, reg_val, dram_ndx, 24, exit);

  reg_val = 0x80;
  SOC_PETRA_FLD_ISET(regs->drc.ac_operating_conditions1_reg.ddrt_dll, reg_val, dram_ndx, 26, exit);

  reg_val = c_info->c_rc;
  SOC_PETRA_FLD_ISET(regs->drc.ac_operating_conditions1_reg.ddrt_rc, reg_val, dram_ndx, 28, exit);

  /* 0x1805  ---------- */
  reg_val = c_info->c_rrd;
  SOC_PETRA_FLD_ISET(regs->drc.ac_operating_conditions2_reg.ddrt_rrd, reg_val, dram_ndx, 30, exit);

  reg_val = c_info->c_rfc;
  SOC_PETRA_FLD_ISET(regs->drc.ac_operating_conditions2_reg.ddrt_rfc, reg_val, dram_ndx, 32, exit);

  reg_val = c_info->c_rcd_rd;
  SOC_PETRA_FLD_ISET(regs->drc.ac_operating_conditions2_reg.ddrt_rcdrd, reg_val, dram_ndx, 34, exit);

  reg_val = c_info->c_rcd_wr;
  SOC_PETRA_FLD_ISET(regs->drc.ac_operating_conditions2_reg.ddrt_rcdwr, reg_val, dram_ndx, 36, exit);

  /* 0x1806  0x00000030 */
  reg_val = 0x00000030;
  SOC_PETRA_REG_ISET(regs->drc.init_sequence_reg, reg_val, dram_ndx, 38, exit);

  /* 0x1807  ---------- */
  reg_val = c_info->c_ras_rd;
  SOC_PETRA_FLD_ISET(regs->drc.ac_operating_conditions3_reg.cnt_rasrdprd, reg_val, dram_ndx, 40, exit);

  reg_val = c_info->c_ras_wr;
  SOC_PETRA_FLD_ISET(regs->drc.ac_operating_conditions3_reg.cnt_raswrprd, reg_val, dram_ndx, 42, exit);

  reg_val = c_info->c_rd_ap;
  SOC_PETRA_FLD_ISET(regs->drc.ac_operating_conditions3_reg.cnt_rdapprd, reg_val, dram_ndx, 44, exit);

  reg_val = c_info->c_wr_ap;
  SOC_PETRA_FLD_ISET(regs->drc.ac_operating_conditions3_reg.cnt_wrapprd, reg_val, dram_ndx, 46, exit);

  /* 0x1809  0x0a803fff */
  reg_val = 0x0a803fff;
  SOC_PETRA_REG_ISET(regs->drc.training_sequence_reg, reg_val, dram_ndx, 48, exit);

  /* 0x180a  0x11111111 */
  reg_val = 0x11111111;
  SOC_PETRA_REG_ISET(regs->drc.training_sequence_word_reg[0], reg_val, dram_ndx, 50, exit);

  /* 0x180b  0x22222222 */
  reg_val = 0x22222222;
  SOC_PETRA_REG_ISET(regs->drc.training_sequence_word_reg[1], reg_val, dram_ndx, 52, exit);

  /* 0x180c  0x33333333 */
  reg_val = 0x33333333;
  SOC_PETRA_REG_ISET(regs->drc.training_sequence_word_reg[2], reg_val, dram_ndx, 54, exit);

  /* 0x180d  0x44444444 */
  reg_val = 0x44444444;
  SOC_PETRA_REG_ISET(regs->drc.training_sequence_word_reg[3], reg_val, dram_ndx, 56, exit);

  /* 0x180e  0x55555555 */
  reg_val = 0x55555555;
  SOC_PETRA_REG_ISET(regs->drc.training_sequence_word_reg[4], reg_val, dram_ndx, 58, exit);

  /* 0x180f  0x66666666 */
  reg_val = 0x66666666;
  SOC_PETRA_REG_ISET(regs->drc.training_sequence_word_reg[5], reg_val, dram_ndx, 60, exit);

  /* 0x1810  0x77777777 */
  reg_val = 0x77777777;
  SOC_PETRA_REG_ISET(regs->drc.training_sequence_word_reg[6], reg_val, dram_ndx, 62, exit);

  /* 0x1811  0x88888888 */
  reg_val = 0x88888888;
  SOC_PETRA_REG_ISET(regs->drc.training_sequence_word_reg[7], reg_val, dram_ndx, 64, exit);

  /* 0x1813  ---------- */
  reg_val = c_info->nof_cols;
  SOC_PETRA_FLD_ISET(regs->drc.drc_general_configurations_reg.num_cols, reg_val, dram_ndx, 66, exit);

  reg_val = c_info->ap_bit_pos;
  SOC_PETRA_FLD_ISET(regs->drc.drc_general_configurations_reg.apbit_pos, reg_val, dram_ndx, 68, exit);

  reg_val = soc_petra_dram_internal_refresh_delay_period_get(unit, dram_freq);
  SOC_PETRA_FLD_ISET(regs->drc.drc_general_configurations_reg.refresh_delay_prd, reg_val, dram_ndx, 70, exit);

  /* 0x1814  ---------- */
  reg_val = 0x40;
  SOC_PETRA_FLD_ISET(regs->drc.write_read_rates_reg.ddrt_zqcs, reg_val, dram_ndx, 72, exit);

  reg_val = c_info->c_faw;
  SOC_PETRA_FLD_ISET(regs->drc.write_read_rates_reg.ddrt_faw, reg_val, dram_ndx, 74, exit);

  /* 0x1815  ---------- */
  reg_val = c_info->c_wr_latency;
  SOC_PETRA_FLD_ISET(regs->drc.dram_compliance_configuration_reg.wr_latency, reg_val, dram_ndx, 76, exit);

  reg_val = (c_info->burst_size == SOC_PETRA_DRAM_BURST_SIZE_16);
  SOC_PETRA_FLD_ISET(regs->drc.dram_compliance_configuration_reg.burst_size_mode, reg_val, dram_ndx, 78, exit);

  reg_val = dram_type;
  SOC_PETRA_FLD_ISET(regs->drc.dram_compliance_configuration_reg.dramtype, reg_val, dram_ndx, 80, exit);

  reg_val = (c_info->nof_banks == SOC_PETRA_DRAM_NUM_BANKS_8);
  SOC_PETRA_FLD_ISET(regs->drc.dram_compliance_configuration_reg.enable8_banks, reg_val, dram_ndx, 82, exit);

  reg_val = 0x0;
  SOC_PETRA_FLD_ISET(regs->drc.dram_compliance_configuration_reg.static_odt_en, reg_val, dram_ndx, 84, exit);

  reg_val = 0x0;
  SOC_PETRA_FLD_ISET(regs->drc.dram_compliance_configuration_reg.addr_term_half, reg_val, dram_ndx, 86, exit);

  /* 0x181a  0x0b0f003d */
  reg_val = c_info->c_ref;
  SOC_PETRA_FLD_ISET(regs->drc.ac_operating_conditions4_reg.ddrt_refi, reg_val, dram_ndx, 90, exit);

  reg_val = c_info->c_wr_rd;
  SOC_PETRA_FLD_ISET(regs->drc.ac_operating_conditions4_reg.cnt_wrrdprd, reg_val, dram_ndx, 92, exit);

  reg_val = c_info->c_rd_wr;
  SOC_PETRA_FLD_ISET(regs->drc.ac_operating_conditions4_reg.cnt_rdwrprd, reg_val, dram_ndx, 94, exit);

  /* 0x181b  0x06010000 */
  reg_val = 0x06010000;
  SOC_PETRA_REG_ISET(regs->drc.odt_configuration_reg, reg_val, dram_ndx, 96, exit);

  /* 0x0c81  0x00180e45 */
  reg_val = 0x00180e45;
  SOC_PETRA_REG_ISET(regs->dpi.io_odt_read_config_reg, reg_val, dram_ndx, 98, exit);

  /* 0x0c64  0x00000001 */
  reg_val = 0x00000011;
  SOC_PETRA_REG_ISET(regs->dpi.ddl_periodic_training_reg, reg_val, dram_ndx, 100, exit);

  for (reg_idx = 0; reg_idx < SOC_PETRA_DRAM_NOF_DLL_REGS ; ++reg_idx)
  {
    /* 0x0c66  0x400e4010 */
    reg_val = 0x400e4010;
    SOC_PETRA_REG_ISET( regs->dpi.ddl_ctrl_reg_1[reg_idx], reg_val, dram_ndx, 102, exit);

    /* 0x0c67  0x0000001f */
    reg_val = 0x0000001f;
    SOC_PETRA_REG_ISET( regs->dpi.ddl_ctrl_reg_2[reg_idx], reg_val, dram_ndx, 104, exit);
  }

  /* 0x0c83  0x00000004 */
  reg_val = (dram_type == SOC_PETRA_DRAM_TYPE_GDDR3);
  SOC_PETRA_FLD_ISET(regs->dpi.ddr_phy_mode_reg.gddr3_mode, reg_val, dram_ndx, 106, exit);

  reg_val = (dram_type == SOC_PETRA_DRAM_TYPE_DDR3);
  SOC_PETRA_FLD_ISET(regs->dpi.ddr_phy_mode_reg.ddr3_mode, reg_val, dram_ndx, 108, exit);

  /* 0x0c61  0x00000001 */
  reg_val = 0x00000001;
  SOC_PETRA_REG_ISET(regs->dpi.dpi_init_reset_reg, reg_val, dram_ndx, 110, exit);

  /* 0x0c60  0x00000001 */
  reg_val = 0x00000001;
  SOC_PETRA_REG_ISET(regs->dpi.dpi_init_start_reg, reg_val, dram_ndx, 112, exit);

  /* 0x1800  0x00000003 */
  reg_val = 0x00000003;
  SOC_PETRA_REG_ISET(regs->drc.ddr_controller_triggers_reg, reg_val, dram_ndx, 114, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dram_info_set_internal_unsafe()",0,0);
}

STATIC uint32
  soc_petra_dram_PETRA_DRAM_INFO_2_PETRA_DRAM_INTERNAL_INFO(
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  SOC_PETRA_DRAM_INFO           *t_info,
    SOC_SAND_OUT SOC_PETRA_DRAM_INTERNAL_INFO  *c_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    value,
    calc1;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(t_info);
  SOC_SAND_CHECK_NULL_INPUT(c_info);

  SOC_PETRA_CLEAR(c_info, SOC_PETRA_DRAM_INTERNAL_INFO, 1);

  value = 1;
  c_info->c_pcb = value;

  value = t_info->burst_size;
  c_info->burst_size = value;

  value = t_info->nof_cols;
  c_info->nof_cols = value;

  value = t_info->ap_bit_pos;
  c_info->ap_bit_pos = value;

  value = t_info->nof_banks;
  c_info->nof_banks = value;

  value = t_info->c_cas_latency;
  c_info->c_cas_latency = value;

  value = t_info->c_wr_latency;
  c_info->c_wr_latency = value;

  value = SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_rc, TRUE);
  c_info->c_rc = value;

  value = SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_rfc, TRUE);
  c_info->c_rfc = value;

  value = SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_ras, TRUE);
  c_info->c_ras = value;

  value = SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_faw, TRUE );
  c_info->c_faw = value;

  value = SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_rcd_rd, TRUE );
  c_info->c_rcd_rd = value;

  value = SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_rcd_wr, TRUE );
  c_info->c_rcd_wr = value;

  value = SOC_SAND_MAX(SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_rrd, TRUE ), 4);
  c_info->c_rrd = value;

  value = SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_ref, FALSE);
  c_info->c_ref = value/32;

  value = SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_rp, TRUE );
  c_info->c_rp = value;

  value = SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_wr, TRUE );
  c_info->c_wr = ((value == 9) || value == 11) ? value + 1 : value;

  value = SOC_SAND_MAX(SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_wtr, TRUE ), 4);
  c_info->c_wtr = value;

  value = SOC_SAND_MAX(SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_rtp, TRUE ), 4);
  c_info->c_rtp = value;

  value = 0x3fff; /* Max value */
  c_info->c_rst = value;

  value = c_info->c_ras - c_info->burst_size / 2;
  c_info->c_ras_rd = value;

  /* Perform intermediate calculation to compensate for a possible rounding error */
  if (!(SOC_PETRA_DRAM_IS_VAL_IN_CLOCKS(t_info->t_ras) || SOC_PETRA_DRAM_IS_VAL_IN_CLOCKS(t_info->t_wr)))
  {
    calc1 = SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_ras - t_info->t_wr, TRUE );
  }
  else
  {
    calc1 = SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_ras, TRUE) - SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_wr, TRUE );
  }
  
  value = (calc1 <= (c_info->burst_size / 2) + c_info->c_wr_latency ? 1 : calc1 - (c_info->burst_size / 2) - c_info->c_wr_latency);
  c_info->c_ras_wr = value;

  value = (c_info->burst_size / 2) + c_info->c_rp + SOC_SAND_MAX(c_info->c_rtp, 2);
  c_info->c_rd_ap = value;

  value = (c_info->burst_size / 2) + c_info->c_rp + c_info->c_wr + c_info->c_wr_latency;
  c_info->c_wr_ap = value;

  value = (c_info->burst_size / 2) + c_info->c_wtr + c_info->c_wr_latency;
  c_info->c_wr_rd = value;

  value = t_info->c_cas_latency + c_info->c_pcb + 1;
  c_info->c_rd_wr = value;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dram_PETRA_DRAM_INFO_2_PETRA_DRAM_INTERNAL_INFO()",0,0);
}

STATIC uint32
  soc_petra_dram_PETRA_DRAM_MR_INFO_2_PETRA_DRAM_INTERNAL_MR_INFO(
    SOC_SAND_IN  SOC_PETRA_DRAM_TYPE              dram_type,
    SOC_SAND_IN  SOC_PETRA_DRAM_INFO              *t_info,
    SOC_SAND_IN  SOC_PETRA_DRAM_INTERNAL_INFO     *c_info,
    SOC_SAND_OUT SOC_PETRA_DRAM_INTERNAL_MR_INFO  *c_mr_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_DRAM_MODE_REGS
    mode_regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(c_info);
  SOC_SAND_CHECK_NULL_INPUT(c_mr_info);

  SOC_PETRA_CLEAR(c_mr_info, SOC_PETRA_DRAM_INTERNAL_MR_INFO, 1);

  SOC_PETRA_CLEAR(&mode_regs, SOC_PETRA_DRAM_MODE_REGS, 1);
  
  res = soc_petra_mode_regs(
          dram_type,
          &mode_regs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  switch(dram_type)
  {
    case SOC_PETRA_DRAM_TYPE_DDR2:
    {
      if (t_info->auto_mode)
      {
        c_mr_info->mr0 = soc_petra_dram_ddr2_mrs0_get(&mode_regs.ddr2, c_info) | SOC_SAND_SET_BITS_RANGE(0x1, SOC_PETRA_DRAM_MR0_DLL_MSB, SOC_PETRA_DRAM_MR0_DLL_LSB);
        c_mr_info->mr1 = soc_petra_dram_ddr2_mrs0_get(&mode_regs.ddr2, c_info) | SOC_SAND_SET_BITS_RANGE(0x0, SOC_PETRA_DRAM_MR0_DLL_MSB, SOC_PETRA_DRAM_MR0_DLL_LSB);
        c_mr_info->mr2 = soc_petra_dram_ddr2_emr0_get(&mode_regs.ddr2, c_info) | SOC_SAND_SET_BITS_RANGE(0x0, SOC_PETRA_DRAM_EMR1_OCD_MSB, SOC_PETRA_DRAM_EMR1_OCD_LSB);
        c_mr_info->mr3 = soc_petra_dram_ddr2_emr0_get(&mode_regs.ddr2, c_info) | SOC_SAND_SET_BITS_RANGE(0x7, SOC_PETRA_DRAM_EMR1_OCD_MSB, SOC_PETRA_DRAM_EMR1_OCD_LSB);
        c_mr_info->mr4 = soc_petra_dram_ddr2_emr0_get(&mode_regs.ddr2, c_info) | SOC_SAND_SET_BITS_RANGE(0x0, SOC_PETRA_DRAM_EMR1_OCD_MSB, SOC_PETRA_DRAM_EMR1_OCD_LSB);
        c_mr_info->mr5 = soc_petra_dram_ddr2_emr1_get(&mode_regs.ddr2, c_info);
        c_mr_info->mr6 = soc_petra_dram_ddr2_emr2_get(&mode_regs.ddr2, c_info);
      }
      else
      {
        c_mr_info->mr0 = t_info->mode_regs.ddr2.mrs0_wr1;
        c_mr_info->mr1 = t_info->mode_regs.ddr2.mrs0_wr2;
        c_mr_info->mr2 = t_info->mode_regs.ddr2.emr0_wr1;
        c_mr_info->mr3 = t_info->mode_regs.ddr2.emr0_wr2;
        c_mr_info->mr4 = t_info->mode_regs.ddr2.emr0_wr3;
        c_mr_info->mr5 = t_info->mode_regs.ddr2.emr1_wr1;
        c_mr_info->mr6 = t_info->mode_regs.ddr2.emr2_wr1;
      }
      break;
    }
    case SOC_PETRA_DRAM_TYPE_DDR3:
    {
      if (t_info->auto_mode)
      {
        c_mr_info->mr0 = soc_petra_dram_ddr3_mrs0_get(&mode_regs.ddr3, c_info) | SOC_SAND_SET_BITS_RANGE(0x1, SOC_PETRA_DRAM_MR0_DLL_MSB, SOC_PETRA_DRAM_MR0_DLL_LSB);
        c_mr_info->mr1 = soc_petra_dram_ddr3_mrs0_get(&mode_regs.ddr3, c_info) | SOC_SAND_SET_BITS_RANGE(0x0, SOC_PETRA_DRAM_MR0_DLL_MSB, SOC_PETRA_DRAM_MR0_DLL_LSB);
        c_mr_info->mr2 = soc_petra_dram_ddr3_mrs1_get(&mode_regs.ddr3, c_info);
        c_mr_info->mr3 = 0x0;
        c_mr_info->mr4 = 0x0;
        c_mr_info->mr5 = soc_petra_dram_ddr3_mrs2_get(&mode_regs.ddr3, c_info);
        c_mr_info->mr6 = soc_petra_dram_ddr3_mrs3_get(&mode_regs.ddr3, c_info);
      }
      else
      {
        c_mr_info->mr0 = t_info->mode_regs.ddr3.mrs0_wr1;
        c_mr_info->mr1 = t_info->mode_regs.ddr3.mrs0_wr2;
        c_mr_info->mr2 = t_info->mode_regs.ddr3.mrs1_wr1;
        c_mr_info->mr3 = 0x0;
        c_mr_info->mr4 = 0x0;
        c_mr_info->mr5 = t_info->mode_regs.ddr3.mrs2_wr1;
        c_mr_info->mr6 = t_info->mode_regs.ddr3.mrs3_wr1;
      }
      break;
    }
    case SOC_PETRA_DRAM_TYPE_GDDR3:
    {
      if (t_info->auto_mode)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DRAM_AUTO_MODE_IS_NOT_SUPPORTED_ERR, 5, exit);
      }
      else
      {
        c_mr_info->mr0 = t_info->mode_regs.gdd3.mrs0_wr1;
        c_mr_info->mr1 = 0x0;
        c_mr_info->mr2 = t_info->mode_regs.gdd3.emr0_wr1;
        c_mr_info->mr3 = 0x0;
        c_mr_info->mr4 = 0x0;
        c_mr_info->mr5 = 0x0;
        c_mr_info->mr6 = 0x0;
      }
      break;
    }
    default:
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DRAM_INVALID_DRAM_TYPE_ERR, 40, exit)
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dram_PETRA_DRAM_MR_INFO_2_PETRA_DRAM_INTERNAL_MR_INFO()",0,0);
}

/*********************************************************************
*  This function configures the dram according to the
*  provided sets of parameters
*  Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dram_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  SOC_PETRA_DRAM_TYPE           dram_type,
    SOC_SAND_IN  SOC_PETRA_DRAM_INFO           *t_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_DRAM_INTERNAL_INFO
    c_info;
  SOC_PETRA_DRAM_INTERNAL_MR_INFO
    c_mr_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DRAM_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(t_info);

  SOC_SAND_ERR_IF_ABOVE_MAX(dram_ndx, SOC_PETRA_BLK_NOF_INSTANCES_DPI, SOC_PETRA_DIAG_DRAM_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_PETRA_CLEAR(&c_info, SOC_PETRA_DRAM_INTERNAL_INFO, 1);
  SOC_PETRA_CLEAR(&c_mr_info, SOC_PETRA_DRAM_INTERNAL_MR_INFO, 1);

  /* Convert values from nanoseconds to clocks */
  res = soc_petra_dram_PETRA_DRAM_INFO_2_PETRA_DRAM_INTERNAL_INFO(
          dram_freq,
          t_info,
          &c_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  /* Convert values from nanoseconds to clocks */
  res = soc_petra_dram_PETRA_DRAM_MR_INFO_2_PETRA_DRAM_INTERNAL_MR_INFO(
          dram_type,
          t_info,
          &c_info,
          &c_mr_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = soc_petra_dram_info_set_internal_unsafe(
          unit,
          dram_ndx,
          dram_freq,
          dram_type,
          &c_info,
          &c_mr_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dram_info_set_unsafe()",0,0);
}

/*********************************************************************
*  This function configures the dram according to the
*  provided sets of parameters
*  Details: in the H file. (search for prototype)
*********************************************************************/
STATIC uint32
  soc_petra_dram_info_gdd3_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  SOC_PETRA_DRAM_INTERNAL_INFO  *c_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DRAM_INFO_VERIFY);

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dram_info_gdd3_verify()",0,0);
}

/*********************************************************************
*  This function configures the dram according to the
*  provided sets of parameters
*  Details: in the H file. (search for prototype)
*********************************************************************/
STATIC uint32
  soc_petra_dram_info_ddr2_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  SOC_PETRA_DRAM_INTERNAL_INFO  *c_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DRAM_INFO_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(c_info->c_cas_latency, SOC_PETRA_DRAM_DDR2_CL_MIN, SOC_PETRA_DRAM_DDR2_CL_MAX, SOC_PETRA_DRAM_CL_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(c_info->c_wr, SOC_PETRA_DRAM_DDR2_WR_MIN, SOC_PETRA_DRAM_DDR2_WR_MAX, SOC_PETRA_DRAM_WR_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dram_info_ddr2_verify()",0,0);
}

/*********************************************************************
*  This function configures the dram according to the
*  provided sets of parameters
*  Details: in the H file. (search for prototype)
*********************************************************************/
STATIC uint32
  soc_petra_dram_info_ddr3_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  SOC_PETRA_DRAM_INTERNAL_INFO  *c_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DRAM_INFO_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(c_info->c_wr_latency, SOC_PETRA_DRAM_DDR3_WL_MIN, SOC_PETRA_DRAM_DDR3_WL_MAX, SOC_PETRA_DRAM_WL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(c_info->c_cas_latency, SOC_PETRA_DRAM_DDR3_CL_MIN, SOC_PETRA_DRAM_DDR3_CL_MAX, SOC_PETRA_DRAM_CL_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(c_info->c_wr, SOC_PETRA_DRAM_DDR3_WR_MIN, SOC_PETRA_DRAM_DDR3_WR_MAX, SOC_PETRA_DRAM_WR_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_EQUALS_VALUE(c_info->burst_size, SOC_PETRA_DRAM_BURST_SIZE_16, SOC_PETRA_DRAM_BL_OUT_OF_RANGE_ERR, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dram_info_ddr3_verify()",0,0);
}

/*********************************************************************
*  This function configures the dram according to the
*  provided sets of parameters
*  Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dram_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  SOC_PETRA_DRAM_TYPE           dram_type,
    SOC_SAND_IN  SOC_PETRA_DRAM_INFO           *t_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_DRAM_INTERNAL_INFO
    c_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DRAM_INFO_VERIFY);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(t_info);

  SOC_PETRA_CLEAR(&c_info, SOC_PETRA_DRAM_INTERNAL_INFO, 1);

  /*
   *  Since there's no JEDEK for GDDR3, user is required to
   *  explicitly provide the mode registers values
   */
  if ((dram_type == SOC_PETRA_DRAM_TYPE_GDDR3) && (t_info->auto_mode == TRUE))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DRAM_AUTO_MODE_IS_NOT_SUPPORTED_ERR, 5, exit);
  }

  /* Convert values from nanoseconds to clocks */
  res = soc_petra_dram_PETRA_DRAM_INFO_2_PETRA_DRAM_INTERNAL_INFO(
          dram_freq,
          t_info,
          &c_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  switch(dram_type)
  {
    case SOC_PETRA_DRAM_TYPE_DDR2:
    {
      res = soc_petra_dram_info_ddr2_verify(
            unit,
            dram_freq,
            &c_info
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit)
      break;
    }
    case SOC_PETRA_DRAM_TYPE_DDR3:
    {
      res = soc_petra_dram_info_ddr3_verify(
            unit,
            dram_freq,
            &c_info
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit)
      break;
    }
    case SOC_PETRA_DRAM_TYPE_GDDR3:
    {
      res = soc_petra_dram_info_gdd3_verify(
            unit,
            dram_freq,
            &c_info
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit)
      break;
    }
    default:
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DRAM_INVALID_DRAM_TYPE_ERR, 40, exit)
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dram_info_verify()",0,0);
}


/*********************************************************************
*  This function configures the dram according to the
*  provided sets of parameters
*  Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dram_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_OUT SOC_PETRA_DRAM_TYPE           *dram_type,
    SOC_SAND_OUT SOC_PETRA_DRAM_INFO           *t_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    reg_val;
  uint32
    c_pcb = 1;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DRAM_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(t_info);

  SOC_PETRA_REG_IGET(regs->drc.dram_compliance_configuration_reg, reg_val, dram_ndx, 10, exit);
  *dram_type = SOC_PETRA_FLD_FROM_PLACE(reg_val, SOC_PETRA_REG_DB_ACC(regs->drc.dram_compliance_configuration_reg.dramtype));

  t_info->auto_mode = FALSE;

  if (*dram_type == SOC_PETRA_DRAM_TYPE_DDR2)
  {
    /* 0x1801 */
    SOC_PETRA_REG_IGET(regs->drc.ddr_mode_1_reg, reg_val, dram_ndx, 12, exit);
    t_info->mode_regs.ddr2.mrs0_wr1 = reg_val;
    /* 0x1802 */
    SOC_PETRA_REG_IGET(regs->drc.ddr_mode_2_reg, reg_val, dram_ndx, 14, exit);
    t_info->mode_regs.ddr2.mrs0_wr2 = reg_val;
    /* 0x1803 */
    SOC_PETRA_REG_IGET(regs->drc.ddr_extended_mode_1_reg, reg_val, dram_ndx, 16, exit);
    t_info->mode_regs.ddr2.emr0_wr1 = reg_val;
    /* 0x1816 */
    SOC_PETRA_REG_IGET(regs->drc.extended_mode_wr2_reg, reg_val, dram_ndx, 12, exit);
    t_info->mode_regs.ddr2.emr0_wr2 = reg_val;
    /* 0x1817 */
    SOC_PETRA_REG_IGET(regs->drc.ddr2_extended_mode_wr3_reg, reg_val, dram_ndx, 14, exit);
    t_info->mode_regs.ddr2.emr0_wr3 = reg_val;
    /* 0x1818 */
    SOC_PETRA_REG_IGET(regs->drc.ddr_extended_mode_2_reg, reg_val, dram_ndx, 16, exit);
    t_info->mode_regs.ddr2.emr1_wr1 = reg_val;
    /* 0x1819 */
    SOC_PETRA_REG_IGET(regs->drc.ddr_extended_mode_3_reg, reg_val, dram_ndx, 16, exit);
    t_info->mode_regs.ddr2.emr2_wr1 = reg_val;
  }
  if (*dram_type == SOC_PETRA_DRAM_TYPE_DDR3)
  {
    /* 0x1801 */
    SOC_PETRA_REG_IGET(regs->drc.ddr_mode_1_reg, reg_val, dram_ndx, 12, exit);
    t_info->mode_regs.ddr3.mrs0_wr1 = reg_val;
    /* 0x1802 */
    SOC_PETRA_REG_IGET(regs->drc.ddr_mode_2_reg, reg_val, dram_ndx, 14, exit);
    t_info->mode_regs.ddr3.mrs0_wr2 = reg_val;
    /* 0x1803 */
    SOC_PETRA_REG_IGET(regs->drc.ddr_extended_mode_1_reg, reg_val, dram_ndx, 16, exit);
    t_info->mode_regs.ddr3.mrs1_wr1 = reg_val;
    /* 0x1818 */
    SOC_PETRA_REG_IGET(regs->drc.ddr_extended_mode_2_reg, reg_val, dram_ndx, 16, exit);
    t_info->mode_regs.ddr3.mrs2_wr1 = reg_val;
    /* 0x1819 */
    SOC_PETRA_REG_IGET(regs->drc.ddr_extended_mode_3_reg, reg_val, dram_ndx, 16, exit);
    t_info->mode_regs.ddr3.mrs3_wr1 = reg_val;
  }
  if (*dram_type == SOC_PETRA_DRAM_TYPE_GDDR3)
  {
    /* 0x1801 */
    SOC_PETRA_REG_IGET(regs->drc.ddr_mode_1_reg, reg_val, dram_ndx, 12, exit);
    t_info->mode_regs.gdd3.mrs0_wr1 = reg_val;
    /* 0x1803 */
    SOC_PETRA_REG_IGET(regs->drc.ddr_extended_mode_1_reg, reg_val, dram_ndx, 16, exit);
    t_info->mode_regs.gdd3.emr0_wr1 = reg_val;
  }
  

  SOC_PETRA_REG_IGET(regs->drc.dram_compliance_configuration_reg, reg_val, dram_ndx, 18, exit);
  t_info->burst_size = SOC_PETRA_FLD_FROM_PLACE(reg_val, SOC_PETRA_REG_DB_ACC(regs->drc.dram_compliance_configuration_reg.burst_size_mode)) ? SOC_PETRA_DRAM_BURST_SIZE_16 : SOC_PETRA_DRAM_BURST_SIZE_32;

  SOC_PETRA_REG_IGET(regs->drc.dram_compliance_configuration_reg, reg_val, dram_ndx, 22, exit);
  t_info->nof_banks = SOC_PETRA_FLD_FROM_PLACE(reg_val, SOC_PETRA_REG_DB_ACC(regs->drc.dram_compliance_configuration_reg.enable8_banks)) ? SOC_PETRA_DRAM_NUM_BANKS_8 : SOC_PETRA_DRAM_NUM_BANKS_4;

  SOC_PETRA_REG_IGET(regs->drc.ac_operating_conditions4_reg, reg_val, dram_ndx, 24, exit);
  t_info->c_cas_latency = SOC_PETRA_FLD_FROM_PLACE(reg_val, SOC_PETRA_REG_DB_ACC(regs->drc.ac_operating_conditions4_reg.cnt_rdwrprd)) - c_pcb - 1;

  SOC_PETRA_REG_IGET(regs->drc.dram_compliance_configuration_reg, reg_val, dram_ndx, 26, exit);
  t_info->c_wr_latency = SOC_PETRA_FLD_FROM_PLACE(reg_val, SOC_PETRA_REG_DB_ACC(regs->drc.dram_compliance_configuration_reg.wr_latency));

  SOC_PETRA_REG_IGET(regs->drc.ac_operating_conditions1_reg, reg_val, dram_ndx, 28, exit);
  t_info->t_rc = soc_petra_64bit_dram_clocks_to_val(TRUE, SOC_PETRA_FLD_FROM_PLACE(reg_val, SOC_PETRA_REG_DB_ACC(regs->drc.ac_operating_conditions1_reg.ddrt_rc)),1000000,dram_freq);

  SOC_PETRA_REG_IGET(regs->drc.ac_operating_conditions2_reg, reg_val, dram_ndx, 30, exit);
  t_info->t_rfc = soc_petra_64bit_dram_clocks_to_val(TRUE, SOC_PETRA_FLD_FROM_PLACE(reg_val, SOC_PETRA_REG_DB_ACC(regs->drc.ac_operating_conditions2_reg.ddrt_rfc)),1000000,dram_freq);

  SOC_PETRA_REG_IGET(regs->drc.ac_operating_conditions3_reg, reg_val, dram_ndx, 32, exit);
  t_info->t_ras = SOC_PETRA_FLD_FROM_PLACE(reg_val, SOC_PETRA_REG_DB_ACC(regs->drc.ac_operating_conditions3_reg.cnt_rasrdprd)) + t_info->burst_size / 2;

  SOC_PETRA_REG_IGET(regs->drc.write_read_rates_reg, reg_val, dram_ndx, 34, exit);
  t_info->t_faw = soc_petra_64bit_dram_clocks_to_val(TRUE, SOC_PETRA_FLD_FROM_PLACE(reg_val, SOC_PETRA_REG_DB_ACC(regs->drc.write_read_rates_reg.ddrt_faw)), 1000000, dram_freq);

  SOC_PETRA_REG_IGET(regs->drc.ac_operating_conditions2_reg, reg_val, dram_ndx, 36, exit);
  t_info->t_rcd_rd = soc_petra_64bit_dram_clocks_to_val(TRUE, SOC_PETRA_FLD_FROM_PLACE(reg_val, SOC_PETRA_REG_DB_ACC(regs->drc.ac_operating_conditions2_reg.ddrt_rcdrd)),1000000, dram_freq);

  SOC_PETRA_REG_IGET(regs->drc.ac_operating_conditions2_reg, reg_val, dram_ndx, 38, exit);
  t_info->t_rcd_wr = soc_petra_64bit_dram_clocks_to_val(TRUE, SOC_PETRA_FLD_FROM_PLACE(reg_val, SOC_PETRA_REG_DB_ACC(regs->drc.ac_operating_conditions2_reg.ddrt_rcdwr)),1000000, dram_freq);

  SOC_PETRA_REG_IGET(regs->drc.ac_operating_conditions2_reg, reg_val, dram_ndx, 40, exit);
  t_info->t_rrd = soc_petra_64bit_dram_clocks_to_val(TRUE, SOC_PETRA_FLD_FROM_PLACE(reg_val, SOC_PETRA_REG_DB_ACC(regs->drc.ac_operating_conditions2_reg.ddrt_rrd)),1000000, dram_freq);

  SOC_PETRA_REG_IGET(regs->drc.ac_operating_conditions4_reg, reg_val, dram_ndx, 42, exit);
  t_info->t_ref = soc_petra_64bit_dram_clocks_to_val(FALSE, 32 * SOC_PETRA_FLD_FROM_PLACE(reg_val, SOC_PETRA_REG_DB_ACC(regs->drc.ac_operating_conditions4_reg.ddrt_refi)),1000000, dram_freq);

  SOC_PETRA_REG_IGET(regs->drc.ac_operating_conditions3_reg, reg_val, dram_ndx, 44, exit);
  t_info->t_wr = soc_petra_64bit_dram_clocks_to_val(TRUE, SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_ras,1) + (t_info->burst_size / 2) + t_info->c_wr_latency - SOC_PETRA_FLD_FROM_PLACE(reg_val, SOC_PETRA_REG_DB_ACC(regs->drc.ac_operating_conditions3_reg.cnt_raswrprd)), 1000000, dram_freq);

  SOC_PETRA_REG_IGET(regs->drc.ac_operating_conditions3_reg, reg_val, dram_ndx, 46, exit);
  t_info->t_rp = soc_petra_64bit_dram_clocks_to_val(TRUE, (SOC_PETRA_FLD_FROM_PLACE( reg_val, SOC_PETRA_REG_DB_ACC(regs->drc.ac_operating_conditions3_reg.cnt_wrapprd)) - (t_info->burst_size / 2) - SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_wr , 1) - t_info->c_wr_latency), 1000000, dram_freq);

  SOC_PETRA_REG_IGET(regs->drc.ac_operating_conditions4_reg, reg_val, dram_ndx, 48, exit);
  t_info->t_wtr = soc_petra_64bit_dram_clocks_to_val(TRUE, SOC_PETRA_FLD_FROM_PLACE( reg_val, SOC_PETRA_REG_DB_ACC(regs->drc.ac_operating_conditions4_reg.cnt_wrrdprd)) - (t_info->burst_size / 2) - t_info->c_wr_latency, 1000000, dram_freq);

  SOC_PETRA_REG_IGET(regs->drc.ac_operating_conditions3_reg, reg_val, dram_ndx, 50, exit);
  t_info->t_rtp = soc_petra_64bit_dram_clocks_to_val(TRUE, SOC_PETRA_FLD_FROM_PLACE( reg_val, SOC_PETRA_REG_DB_ACC(regs->drc.ac_operating_conditions3_reg.cnt_rdapprd)) - (t_info->burst_size / 2) - SOC_PETRA_DRAM_VAL_TO_CLOCKS(t_info->t_rp,1), 1000000, dram_freq);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dram_info_get_unsafe()",0,0);
}


/*********************************************************************
 *  This function calculates clock value that will be written to the clock register
 *  Calculation is done in 64 bit resolution 
 *********************************************************************/
STATIC uint32
  soc_petra_64bit_dram_clocks_to_val(
   SOC_SAND_IN  uint8           is_round_up,
   SOC_SAND_IN  uint32           value,
    SOC_SAND_IN  uint32           mult2,
   SOC_SAND_IN  uint32           dram_freq
  )
{
  SOC_SAND_U64
    u64_tmp,
    u64_tmp_res;
  uint32
    ret_val;
  
  soc_sand_u64_clear(&u64_tmp);
  soc_sand_u64_clear(&u64_tmp_res);

  soc_sand_u64_multiply_longs(
    value,
    mult2,
    &u64_tmp
  );
  
  if ((is_round_up == TRUE) && (dram_freq != 0))
  {
    soc_sand_u64_add_long(&u64_tmp, dram_freq - 1);
  }
  soc_sand_u64_devide_u64_long( &u64_tmp, dram_freq, &u64_tmp_res);
  ret_val = u64_tmp_res.arr[0];

#if SOC_PETRA_DEBUG_IS_LVL2
  if (u64_tmp_res.arr[1] != 0)
  {
    soc_sand_os_printf(
      "soc_petra_64bit_dram_clocks_to_val: Internal calculation overflow ((%u*%u)/%u) "
      "\n\r",
      value, mult2,dram_freq
    );
  }
#endif
  return (ret_val); 
}



/*********************************************************************
*     Diagnostic tool to get the DRAM valid window size
 *     through BIST commands. .
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dram_window_validity_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DRAM_PATTERN                        *pattern,
    SOC_SAND_OUT SOC_PETRA_DRAM_WINDOW_INFO                    *window_info
  )
{
  uint32
    fld_val,
    fld_val1,
    sampling_point,
    entry_offset = SOC_PETRA_DRAM_RAM_ENTRY_OFFSET,
    single_interface_dram_size_bytes,
    quarter_cycle_reg_ndx,
    quarter_cycle_avg,
    reg_ndx,
    tap_ndx,
    tap2_ndx,
    boundary_right = 0,
    boundary_left = 0,
    ram_val_ndx,
    ram_val_fld_ndx,
    pattern_ndx,
    reg_val,
    nof_reads = 15,
    nof_writes = 17,
    pattern_val[SOC_PETRA_DRAM_PATTERN_SIZE_IN_UINT32S],
    dummy_bist_pattern[SOC_PETRA_DRAM_PATTERN_SIZE_IN_UINT32S],
    ram_val[SOC_PETRA_DRAM_RAM_VAL_IN_UINT32S],
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_DIAG_DRAM_ACCESS_INFO
    dram_access;
  SOC_PETRA_DIAG_DRAM_READ_COMPARE_STATUS
    read_status;
  uint8
    is_first_tap_right_found = FALSE,
    is_right_found = FALSE,
    res_bitmap[SOC_PETRA_DRAM_NOF_TAPS],
    poll_success;
  SOC_PETRA_DPI_DLL_RAM_TBL_DATA
    dll_ram_tbl_data;
  SOC_PETRA_DRAM_TYPE
    dram_type;
  SOC_PETRA_HW_DRAM_CONF_PARAMS
    dram_conf;
  SOC_PETRA_POLL_INFO
    dpi_poll_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DRAM_WINDOW_VALIDITY_GET_UNSAFE);

  regs = soc_petra_regs();

  SOC_SAND_CHECK_NULL_INPUT(pattern);
  SOC_SAND_CHECK_NULL_INPUT(window_info);

  soc_petra_PETRA_DRAM_WINDOW_INFO_clear(window_info);
  soc_petra_PETRA_DIAG_DRAM_ACCESS_INFO_clear(&dram_access);
  sal_memset(&dram_conf, 0x0, sizeof(SOC_PETRA_HW_DRAM_CONF_PARAMS));

  /*
   *  DPRC init
   */
  res = soc_petra_dbg_dprc_reset_unsafe(
         unit,
         dram_ndx
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  dpi_poll_info.expected_value = 1;
  dpi_poll_info.busy_wait_nof_iters = SOC_PETRA_DRAM_WINDOW_VALIDITY_BUSY_ITERS;
  dpi_poll_info.timer_nof_iters = SOC_PETRA_DRAM_WINDOW_VALIDITY_TIMER_ITERS;
  dpi_poll_info.timer_delay_msec = SOC_PETRA_DRAM_WINDOW_VALIDITY_TIMER_DELAY;
  
  res = soc_petra_status_fld_poll_unsafe(
          unit, 
          SOC_PETRA_REG_DB_ACC_REF(regs->dpi.dpi_init_status_reg.ready),
          dram_ndx,
          &dpi_poll_info, 
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DRAM_DPRC_POLL_FAIL_ERR, 10, exit); 
  }
  

  /*
   *  Initialize the half cycle bitmap
   */
  for (tap_ndx = 0; tap_ndx < SOC_PETRA_DRAM_NOF_TAPS; tap_ndx++)
  {
    res_bitmap[tap_ndx] = FALSE;
  }

  /*
   *  Get the pattern
   */
  if (pattern->mode == SOC_PETRA_DRAM_PATTERN_MODE_USER_DEF)
  {
    for (reg_ndx = 0; reg_ndx < SOC_PETRA_DRAM_PATTERN_SIZE_IN_UINT32S; ++reg_ndx)
    {
      pattern_val[reg_ndx] = pattern->data[reg_ndx];
    }
  }
  else if (pattern->mode == SOC_PETRA_DRAM_PATTERN_MODE_AUTO)
  {
    pattern_val[0] = SOC_PETRA_DRAM_PATTERN_VAL_0;
    pattern_val[1] = SOC_PETRA_DRAM_PATTERN_VAL_1;
    pattern_val[2] = SOC_PETRA_DRAM_PATTERN_VAL_2;
    pattern_val[3] = SOC_PETRA_DRAM_PATTERN_VAL_3;
    pattern_val[4] = SOC_PETRA_DRAM_PATTERN_VAL_4;
    pattern_val[5] = SOC_PETRA_DRAM_PATTERN_VAL_5;
    pattern_val[6] = SOC_PETRA_DRAM_PATTERN_VAL_6;
    pattern_val[7] = SOC_PETRA_DRAM_PATTERN_VAL_7;
 }

  dummy_bist_pattern[0] = SOC_PETRA_DRAM_PATTERN_VAL_0;
  dummy_bist_pattern[1] = SOC_PETRA_DRAM_PATTERN_VAL_1;
  dummy_bist_pattern[2] = SOC_PETRA_DRAM_PATTERN_VAL_2;
  dummy_bist_pattern[3] = SOC_PETRA_DRAM_PATTERN_VAL_3;
  dummy_bist_pattern[4] = SOC_PETRA_DRAM_PATTERN_VAL_4;
  dummy_bist_pattern[5] = SOC_PETRA_DRAM_PATTERN_VAL_5;
  dummy_bist_pattern[6] = SOC_PETRA_DRAM_PATTERN_VAL_6;
  dummy_bist_pattern[7] = SOC_PETRA_DRAM_PATTERN_VAL_7;

  /*
   *  Set the BIST configuration
   */
  dram_access.address = 0;
  dram_access.is_data_size_bits_256_not_32 = TRUE;
  dram_access.is_infinite_nof_actions = TRUE;

  res = soc_petra_sw_db_dram_dram_size_get(
          unit,
          &single_interface_dram_size_bytes
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  single_interface_dram_size_bytes *= SOC_PETRA_DIAG_DRAM_NOF_BYTES_IN_MBYTES;

  SOC_PETRA_FLD_IGET(regs->dpi.dpi_init_status_reg.dll_mstr_s, fld_val, dram_ndx, 20, exit);
  sampling_point = fld_val + SOC_PETRA_DRAM_SHIFT_NOF_TAPS; /* Add 8 due to the window shift */

  /*
   *  Start from the evaluation point (assumption it is located in the DRAM window)
   *  then find the right window boundary and then find the left window boundary
   */
  for (tap2_ndx = 0; tap2_ndx < SOC_PETRA_DRAM_NOF_TAPS; tap2_ndx++)
  {
    if ((tap2_ndx == 0) || (is_first_tap_right_found == TRUE))
    {
      if (is_first_tap_right_found == TRUE)
      {
        is_first_tap_right_found = FALSE;
      }

      SOC_PETRA_DIFF_DEVICE_CALL(mgmt_hw_adjust_ddr_init,(unit, dram_ndx));

      res = soc_petra_sw_db_dram_type_get(
              unit,
              &dram_type
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);

     res = soc_petra_sw_db_dram_conf_get(
              unit,
              &(dram_conf)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);

      res = soc_petra_dram_info_set_unsafe(
              unit,
              dram_ndx,
              dram_conf.dram_freq,
              dram_type,
              &(dram_conf.params)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);
    }
    if (is_right_found == FALSE)
    {
      tap_ndx = sampling_point + tap2_ndx;
    }
    else
    {
      tap_ndx = boundary_right - tap2_ndx;
    }

    res_bitmap[tap_ndx] = TRUE;
  
    /*
     *  Init values
     */
    for (ram_val_ndx = 0; ram_val_ndx < SOC_PETRA_DRAM_RAM_VAL_IN_UINT32S; ++ram_val_ndx)
    {
      ram_val[ram_val_ndx] = 0;
    }

    for (ram_val_fld_ndx = 0; ram_val_fld_ndx < SOC_PETRA_DRAM_RAM_NOF_FLDS; ++ram_val_fld_ndx)
    {
      res = soc_sand_bitstream_set_any_field(
            &(tap_ndx),
            14 * ram_val_fld_ndx,
            6,
            ram_val
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }

    for (ram_val_ndx = 0; ram_val_ndx < SOC_PETRA_DRAM_RAM_VAL_IN_UINT32S; ++ram_val_ndx)
    {
      dll_ram_tbl_data.dll_ram[ram_val_ndx] = ram_val[ram_val_ndx];
    }

    res = soc_petra_dpi_dll_ram_tbl_set_unsafe(
            unit,
            entry_offset,
            dram_ndx,
            &dll_ram_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    /*
     *  Shift of 8 taps for each pattern bit
     */
    for (reg_ndx = 0; reg_ndx < SOC_PETRA_DRAM_NOF_SHIFT_VAL_REGS; ++reg_ndx)
    {
      reg_val = SOC_PETRA_DRAM_SHIFT_VAL;
      SOC_PETRA_REG_ISET(regs->dpi.ddio_bit_align_control_reg[reg_ndx], reg_val, dram_ndx, 50 + reg_ndx, exit);
    }

    /*
     *  Perform the training sequence
     */
    SOC_PETRA_FLD_ISET(regs->dpi.dll_control_reg.dll_control, SOC_PETRA_DRAM_DLL_CONTROL_VAL, dram_ndx, 60, exit);
    SOC_PETRA_FLD_ISET(regs->dpi.ddl_periodic_training_reg.ddl_auto_trn_en, TRUE, dram_ndx, 70, exit);

    res = SOC_SAND_OK; sal_msleep(10);
    SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

    reg_val = 0x1;
    SOC_PETRA_REG_ISET(regs->dpi.dpi_init_reset_reg, reg_val, dram_ndx, 80, exit);
    SOC_PETRA_REG_ISET(regs->dpi.dpi_init_start_reg, reg_val, dram_ndx, 90, exit);

    res = SOC_SAND_OK; sal_msleep(10);
    SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

    /*
     *  Verify the end of the training sequence
     */
    SOC_PETRA_FLD_IGET(regs->dpi.dpi_init_status_reg.ready, fld_val, dram_ndx, 100, exit);
    if (fld_val != 0x1)
    {
      res_bitmap[tap_ndx] = FALSE;
      break;
    }

    SOC_PETRA_FLD_IGET(regs->dpi.dpi_init_status_reg.init_dn, fld_val, dram_ndx, 110, exit);
    if (fld_val != 0xF)
    {
      res_bitmap[tap_ndx] = FALSE;
      break;
    }

    /*
     *  Loop on the different patterns for the auto mode
     */
    for (pattern_ndx = 0; pattern_ndx < 32; ++pattern_ndx)
    {
      if (res_bitmap[tap_ndx] == TRUE)
      {
        /*
         *  Dummy BIST preset to clean the memory
         */
       res = soc_petra_diag_dram_presettings_set(
                unit,
                dram_ndx,
                nof_reads,
                nof_writes
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

        /*
         *  BIST command
         */
        res = soc_petra_diag_dram_access(
                unit,
                dram_ndx,
                &dram_access,
                single_interface_dram_size_bytes - 32,
                dummy_bist_pattern,
                TRUE
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

        /*
         *  Use of infinite number of actions. Wait before getting the
         *  number of errors.
         */
        res = SOC_SAND_OK; sal_msleep(10);
        SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

        /*
         *  Stop the BIST
         */
        SOC_PETRA_FLD_SET(regs->eci.drc_bist_enables_reg.drcs_bist_en[dram_ndx], 0x0 , 150, exit);

        /*
         *  Get the BIST status
         */
        soc_petra_PETRA_DIAG_DRAM_READ_COMPARE_STATUS_clear(&read_status);
        res = soc_petra_diag_dram_read_and_compare_unsafe(
                unit,
                dram_ndx,
                &dram_access,
                dummy_bist_pattern,
                &read_status
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
        /*
         *  Stop the BIST
         */
        SOC_PETRA_FLD_SET(regs->eci.drc_bist_enables_reg.drcs_bist_en[dram_ndx], 0x0 , 170, exit);

        if (read_status.success == FALSE)
        {
          res_bitmap[tap_ndx] = FALSE;
          break;
        }

        /*
         *  BIST R/W
         */
       res = soc_petra_diag_dram_presettings_set(
                unit,
                dram_ndx,
                nof_reads,
                nof_writes
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

        for (reg_ndx = 0; reg_ndx < SOC_PETRA_DRAM_PATTERN_SIZE_IN_UINT32S; ++reg_ndx)
        {
          if (pattern_val[reg_ndx] % 2)
          {
            pattern_val[reg_ndx] = pattern_val[reg_ndx] >> 1;
            pattern_val[reg_ndx] += SOC_PETRA_DRAM_MSB_MASK;
          }
          else
          {
            pattern_val[reg_ndx] = pattern_val[reg_ndx] >> 1;
          }
        }

        /*
         *  BIST command
         */
        res = soc_petra_diag_dram_access(
                unit,
                dram_ndx,
                &dram_access,
                single_interface_dram_size_bytes - 32,
                pattern_val,
                TRUE
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

        /*
         *  Use of infinite number of actions. Wait 1 sec before getting the
         *  number of errors.
         */
        res = SOC_SAND_OK; sal_msleep(10);
        SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

        /*
         *  Stop the BIST
         */
        SOC_PETRA_FLD_SET(regs->eci.drc_bist_enables_reg.drcs_bist_en[dram_ndx], 0x0 , 210, exit);

        /*
         *  Get the BIST status
         */
        soc_petra_PETRA_DIAG_DRAM_READ_COMPARE_STATUS_clear(&read_status);
        res = soc_petra_diag_dram_read_and_compare_unsafe(
                unit,
                dram_ndx,
                &dram_access,
                pattern_val,
                &read_status
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);
        /*
         *  Stop the BIST
         */
        SOC_PETRA_FLD_SET(regs->eci.drc_bist_enables_reg.drcs_bist_en[dram_ndx], 0x0 , 230, exit);


        if (read_status.success == FALSE)
        {
          res_bitmap[tap_ndx] = FALSE;
          break;
        }

      }
    }

    if (res_bitmap[tap_ndx] == FALSE)
    {
     if (is_right_found == FALSE)
      {
        is_right_found = TRUE;
        boundary_right = tap_ndx;
        is_first_tap_right_found = TRUE;
      }
      else
      {
        /*
         *  Left boundary found. Exit the window
         */
        boundary_left = tap_ndx;
        break;
      }
    }
  }

  /*
   *  Set the final results
   */
  SOC_PETRA_FLD_IGET(regs->dpi.dpi_init_status_reg.dll_mstr_s, fld_val, dram_ndx, 280, exit);
  window_info->sampling_point = fld_val + SOC_PETRA_DRAM_SHIFT_NOF_TAPS; /* Add 8 due to the window shift */

  window_info->start = boundary_left + 1;
  window_info->end = boundary_right - 1;

  /*
   *  Get the average value of the half cycles
   */
  quarter_cycle_avg = 0;
  for (quarter_cycle_reg_ndx = 0; quarter_cycle_reg_ndx < SOC_PETRA_DRAM_NOF_HALF_CYCLE_REGS; ++quarter_cycle_reg_ndx)
  {
    SOC_PETRA_FLD_IGET(regs->dpi.ddl1_stat_lsb_reg[quarter_cycle_reg_ndx].qc_sel_vec_0, fld_val, dram_ndx, 290+quarter_cycle_reg_ndx, exit);
    SOC_PETRA_FLD_IGET(regs->dpi.ddl1_stat_msb_reg[quarter_cycle_reg_ndx].qc_sel_vec_1, fld_val1, dram_ndx, 290+quarter_cycle_reg_ndx, exit);

    fld_val = (uint8)(fld_val + (fld_val1 << 7));
    quarter_cycle_avg += fld_val;
  }
  quarter_cycle_avg = quarter_cycle_avg / SOC_PETRA_DRAM_NOF_HALF_CYCLE_REGS;

  window_info->total_size = quarter_cycle_avg * 4;
  window_info->ratio_percent = SOC_PETRA_DRAM_PERCENT_FACTOR * (boundary_right - boundary_left - 1) / window_info->total_size;
  if (window_info->ratio_percent > 100)
  {
    window_info->ratio_percent = 0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dram_window_validity_get_unsafe()", dram_ndx, 0);
}

uint32
  soc_petra_dram_window_validity_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DRAM_PATTERN                        *pattern
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DRAM_WINDOW_VALIDITY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(dram_ndx, SOC_PETRA_BLK_NOF_INSTANCES_DPI, SOC_PETRA_DIAG_DRAM_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(pattern->mode, SOC_PETRA_DRAM_NOF_PATTERN_MODES-1, SOC_PETRA_DIAG_PATTERN_MODE_OUT_OF_RANGE_ERR, 11, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dram_window_validity_get_verify()", dram_ndx, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1
STATIC void
  soc_petra_dram_buff_pckt2chunks(
    SOC_SAND_IN   uint32  pckt_size_in_buff,
    SOC_SAND_OUT  uint32 *nof_chunks_total,
    SOC_SAND_OUT  uint32 *nof_chunks_first,
    SOC_SAND_OUT  uint32  *chunk_before_last_size,
    SOC_SAND_OUT  uint32  *chunk_last_size
  )
{
  uint32 
    num_total = 0,
    num_first = 0,
    last_two_size;
  uint32  
    before_last_size = 0, 
    last_size = 0;

  num_total = pckt_size_in_buff / 128; /* 128 is the "typical" chunk size */
  if ((pckt_size_in_buff % 128) != 0) 
  {
    num_total++;
  }

  if (num_total == 1)   /* there is only one chunk (128 or less)*/
  {
    num_first = 0;
    before_last_size = 0;
    last_size =  pckt_size_in_buff;
    last_two_size = pckt_size_in_buff;    
  }  
  else 
  {
    if ((num_total == 2) && ((pckt_size_in_buff % 128) == 0) ) /* packet is 2*128 size */
    {
      num_first = 1;
      before_last_size = 128;
      last_size = 128;
      last_two_size = 256;
    }
    else /* packet is more then 2*128 size */
    {
      num_first = num_total - 2;
      last_two_size = pckt_size_in_buff - num_first*128;  

      if ((0 < (last_two_size % 128))  && ((last_two_size % 128) < 64)) /* 129 - 191 is 64 and rest */
      {
        before_last_size = 64;
        last_size = last_two_size - 64;
      }      
      else
      {
        if (64 <= (last_two_size % 128))  /* 192 first is 128 second 64 */
        {
          before_last_size = 128;
          last_size = last_two_size - 128;
        } 
      }
    }
  }

  *nof_chunks_total = num_total;
  *nof_chunks_first = num_first;
  *chunk_before_last_size = before_last_size;
  *chunk_last_size = last_size;
}

STATIC uint32
  soc_petra_dram_logic2phys_addr(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  logical_addr,
    SOC_SAND_OUT uint32  *phys_addr
  )
{
  uint32
    res,
    is_8_not_4_banks,
    layer_periodicity,
    same_bank_config,
    addr_for_devision = 0,
    addr_for_devision_lsb,
    layer_num,
    dbuff_mod,
    dbuff_mod_lsb,
    dbuff_mod_aft_xor,
    logical_addr_for_mod,
    dbuff_mod_3,
    layer_num_bits,
    layer_offset,
    nof_bits,
    var1, var2, var3, var4,
    physical_banks;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_GET(regs->mmu.general_configuration_reg.dram_bank_num, is_8_not_4_banks, 10, exit);
  SOC_PETRA_FLD_GET(regs->mmu.bank_access_controller_configurations_reg.address_map_config_gen, same_bank_config, 20, exit);
  SOC_PETRA_FLD_GET(regs->mmu.general_configuration_reg.layer_periodicity, layer_periodicity, 30, exit);

  switch (same_bank_config)
  {
  case 0x0:
    addr_for_devision_lsb = 3;
    dbuff_mod_lsb = 0;
    break;
  case 0x1:
    addr_for_devision_lsb = 4;
    dbuff_mod_lsb = 1;
    break;
  case 0x2:
    addr_for_devision_lsb = 5;
    dbuff_mod_lsb = 2;
    break;
  default:
    addr_for_devision_lsb = 6;
    dbuff_mod_lsb = 3;
  }

  if (is_8_not_4_banks)
  {
    addr_for_devision_lsb++;
  }

  nof_bits = (is_8_not_4_banks)?3:2;
  
  addr_for_devision = SOC_SAND_GET_FLD_FROM_PLACE(logical_addr, addr_for_devision_lsb,
    SOC_SAND_BITS_MASK(31, addr_for_devision_lsb));
  
  layer_num = addr_for_devision / 3 ;

  dbuff_mod = SOC_SAND_GET_FLD_FROM_PLACE(logical_addr, dbuff_mod_lsb,
    SOC_SAND_BITS_MASK(dbuff_mod_lsb+nof_bits, dbuff_mod_lsb));

  layer_num_bits = SOC_SAND_GET_FLD_FROM_PLACE(layer_num, layer_periodicity,
    SOC_SAND_BITS_MASK(layer_periodicity+3, layer_periodicity));
  
  dbuff_mod_aft_xor = dbuff_mod ^ layer_num_bits;

  logical_addr_for_mod = SOC_SAND_GET_FLD_FROM_PLACE(logical_addr, dbuff_mod_lsb,
    SOC_SAND_BITS_MASK(26, dbuff_mod_lsb));

  dbuff_mod_3 = logical_addr_for_mod % 3;

  layer_offset = 3*dbuff_mod_aft_xor + dbuff_mod_3;

  if (is_8_not_4_banks)
  {
    var1 = SOC_SAND_GET_FLD_FROM_PLACE(layer_offset, 0,
      SOC_SAND_BITS_MASK(5, 0));
    switch (same_bank_config)
    {
    case 0x0:
      var2 = 0x0;
      var3 = SOC_SAND_GET_FLD_FROM_PLACE(layer_num, 0,
        SOC_SAND_BITS_MASK(20, 0));
      physical_banks = var1 + (var3 << 6);      
      break;
    case 0x1:
      var2 = SOC_SAND_GET_FLD_FROM_PLACE(logical_addr, 0,
        SOC_SAND_BITS_MASK(0, 0));
      var3 = SOC_SAND_GET_FLD_FROM_PLACE(layer_num, 0,
        SOC_SAND_BITS_MASK(19, 0));
      physical_banks = var1 + (var2 << 6) + (var3 << 7);    
      break;
    case 0x2:
      var2 = SOC_SAND_GET_FLD_FROM_PLACE(logical_addr, 0,
        SOC_SAND_BITS_MASK(1, 0));
      var3 = SOC_SAND_GET_FLD_FROM_PLACE(layer_num, 0,
        SOC_SAND_BITS_MASK(18, 0));
      physical_banks = var1 + (var2 << 6) + (var3 << 8);    
      break;
    default:
      var2 = SOC_SAND_GET_FLD_FROM_PLACE(logical_addr, 0,
        SOC_SAND_BITS_MASK(2, 0));
      var3 = SOC_SAND_GET_FLD_FROM_PLACE(layer_num, 0,
        SOC_SAND_BITS_MASK(17, 0));
      physical_banks = var1 + (var2 << 6) + (var3 << 9);   
    }
  } 
  else
  {
    var1 = SOC_SAND_GET_FLD_FROM_PLACE(layer_offset, 0,
      SOC_SAND_BITS_MASK(1, 0));
    var4 = var1 = SOC_SAND_GET_FLD_FROM_PLACE(layer_offset, 2,
      SOC_SAND_BITS_MASK(4, 2));
    switch (same_bank_config)
    {
    case 0x0:
      var2 = 0x0;
      var3 = SOC_SAND_GET_FLD_FROM_PLACE(layer_num, 0,
        SOC_SAND_BITS_MASK(20, 0));
      physical_banks = var1 + (var4 << 2) + (var3 << 4);      
      break;
    case 0x1:
      var2 = SOC_SAND_GET_FLD_FROM_PLACE(logical_addr, 0,
        SOC_SAND_BITS_MASK(0, 0));
      var3 = SOC_SAND_GET_FLD_FROM_PLACE(layer_num, 0,
        SOC_SAND_BITS_MASK(19, 0));
      physical_banks = var1 + (var4 << 2) + (var2 << 4) + (var3 << 5);    
      break;
    case 0x2:
      var2 = SOC_SAND_GET_FLD_FROM_PLACE(logical_addr, 0,
        SOC_SAND_BITS_MASK(1, 0));
      var3 = SOC_SAND_GET_FLD_FROM_PLACE(layer_num, 0,
        SOC_SAND_BITS_MASK(18, 0));
      physical_banks = var1 + (var4 << 2) + (var2 << 4) + (var3 << 5);    
      break;
    default:
      var2 = SOC_SAND_GET_FLD_FROM_PLACE(logical_addr, 0,
        SOC_SAND_BITS_MASK(2, 0));
      var3 = SOC_SAND_GET_FLD_FROM_PLACE(layer_num, 0,
        SOC_SAND_BITS_MASK(17, 0));
      physical_banks = var1 + (var4 << 2) + (var2 << 4) + (var3 << 5);    
    }
  }

  *phys_addr = physical_banks;
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dram_logic2phys_addr()", 0, 0);
}

void 
  soc_petra_dram_phys_offset_print(
    SOC_SAND_IN uint32  logical_addr,
    SOC_SAND_IN uint32  phys_addr,
    SOC_SAND_IN uint32 data[SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_ENTRY_SIZE]
  )
{
  int32
    i;
  uint32
    data_long;

  soc_sand_os_printf(
    "LA: 0x%.8x, PA: 0x%.8x"
    "\n\r",
    logical_addr,
    phys_addr
  );

  for (i = (SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_ENTRY_SIZE-1); i >= 0; i--)
  {
    data_long = *( (data )+ i);   

    soc_sand_os_printf("%.2x %.2x %.2x %.2x ", (data_long) >> 24, ((data_long) & 0xff0000) >> 16, ((data_long) & 0xff00) >> 8, (data_long) & 0xff);
    if(i%2 == 0)
    {
      soc_sand_os_printf("\n\r");
    }
  }
  soc_sand_os_printf("\n\r");

  /*ret = soc_sand_buff_print_all((uint8*)data, SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_ENTRY_SIZE* sizeof(uint32), 64);*/
}

uint32
  soc_petra_dram_pckt_from_buff_verify(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  buff_id,
    SOC_SAND_IN uint32  pckt_size_in_buff
  )
{
  uint32
    fld_val,
    res;
  SOC_PETRA_ITM_DBUFF_SIZE_BYTES
    dbuff_size;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DRAM_PCKT_FROM_BUFF_READ_VERIFY);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_GET(regs->eci.general_controls_reg.dbuff_size, fld_val, 10, exit);

  res = soc_petra_itm_dbuff_internal2size(
          fld_val,
          &dbuff_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    dbuff_size, SOC_PETRA_ITM_DBUFF_SIZE_BYTES_MIN, SOC_PETRA_ITM_DBUFF_SIZE_BYTES_MAX,
    SOC_PETRA_ITM_DRAM_BUF_SIZE_OUT_OF_RANGE_ERR, 30, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dram_pckt_from_buff_verify()", buff_id, pckt_size_in_buff);
}


uint32
  soc_petra_dram_pckt_from_buff_read_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  buff_id,
    SOC_SAND_IN uint32  pckt_size_in_buff
  )
{
  uint32 
    nof_chunks_total,
    nof_chunks_first,
    chunk_id,
    sub_id,
    nof_sub_ids_in_chunk;
  uint32  
    fld_val,
    dbuff_id_offst,
    sub_offset,
    chunk_before_last_size, 
    chunk_last_size,
    logical_offset,
    physical_offset = 0,
    res,
    data[SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_ENTRY_SIZE];
  SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_DATA 
    MMU_dram_address_space_tbl_data;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL
    *tbl;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DRAM_PCKT_FROM_BUFF_READ_UNSAFE);

  regs = soc_petra_regs();

  res = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = SOC_SAND_OK; sal_memset(
          &MMU_dram_address_space_tbl_data,
          0x0,
          sizeof(SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = SOC_PETRA_TBL_REF(tables->mmu.dram_address_space_tbl);


  soc_petra_dram_buff_pckt2chunks(
    pckt_size_in_buff,
    &nof_chunks_total,
    &nof_chunks_first,
    &chunk_before_last_size,
    &chunk_last_size
  );

  soc_sand_os_printf(
    "\n\r"
    "Printing %u chunks for buffer %u:\n\r"
    "---------------------------------"
    "\n\r",
    nof_chunks_total,
    buff_id
  );
  for (chunk_id = 0; chunk_id < nof_chunks_total; chunk_id++)
  {
    nof_sub_ids_in_chunk = 2;
    if (chunk_id >= nof_chunks_first)
    {
      if (nof_chunks_total == 1)
      {
        /* One chunk */
        nof_sub_ids_in_chunk = (chunk_last_size <= 64)?1:2;
      }
      else
      {
        /* At least two chunks */
        if (chunk_id == nof_chunks_total - 2)
        {
          /* Before last */
          nof_sub_ids_in_chunk = (chunk_before_last_size <= 64)?1:2;
        }
        else if (chunk_id == nof_chunks_total - 1)
        {
          /* Last */
          nof_sub_ids_in_chunk = (chunk_last_size <= 64)?1:2;
        } 
      }        
    } /* (chunk_id >= nof_chunks_first) */

    SOC_PETRA_FLD_GET(regs->eci.general_controls_reg.dbuff_size, fld_val, 10, exit);
    dbuff_id_offst = fld_val+2; /* 2 for 256B (0x0), 3 for 512B (0x1) etc. */

    for (sub_id = 0; sub_id < nof_sub_ids_in_chunk; sub_id++)
    {
      sub_offset = chunk_id * 2 + sub_id;
      logical_offset =  SOC_SAND_GET_FLD_FROM_PLACE(sub_offset, 0,
        SOC_SAND_BITS_MASK(dbuff_id_offst-1, 0));
      logical_offset |= SOC_SAND_SET_FLD_IN_PLACE(buff_id, dbuff_id_offst,
        SOC_SAND_BITS_MASK(31, dbuff_id_offst));

      res = soc_petra_dram_logic2phys_addr(
              unit,
              logical_offset,
              &physical_offset
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      res = soc_sand_tbl_read_unsafe(
              unit,
              data,
              physical_offset,
              sizeof(data),
              SOC_PETRA_MMU_ID,
              SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL_ENTRY_SIZE * sizeof(uint32)
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      res = soc_petra_field_in_place_get(
              data,
              &(tbl->data),
              (MMU_dram_address_space_tbl_data.data)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      
      soc_petra_dram_phys_offset_print(logical_offset, physical_offset, data);

    } /* For (sub-chunks up to 64B) */
    
  } /* For(chunks) */
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dram_pckt_from_buff_read_unsafe()", buff_id, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

